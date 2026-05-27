// ═══════════════════════════════════════════════════════════════
// CIRCADIA-SENSE  v3.0 — Bluetooth Serial Control
//
// Core 0 : WiFi + NTP + Weather + BluetoothSerial (command parser)
// Core 1 : All Sensors + LED + OLED + Serial/BT print
//
// ── BT COMMANDS (type in any Bluetooth Serial Terminal app) ──
//   COLOR:WARM       → 2700K warm white
//   COLOR:COOL       → 6000K cool white
//   COLOR:WHITE      → 5500K bright white
//   COLOR:AMBER      → 2000K amber/orange
//   BRIGHT:0–100     → set brightness (e.g. BRIGHT:75)
//   COLOR:WARM,75    → color + brightness in one command
//   STATUS           → print current sensor snapshot
//   HELP             → list all commands
//
// ── SETUP ────────────────────────────────────────────────────
//   Pair phone to "CIRCADIA-BT" (no PIN) via Bluetooth settings
//   Open any BT Serial Terminal app and connect
// ═══════════════════════════════════════════════════════════════

#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Adafruit_APDS9960.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "BluetoothSerial.h"        // ← Classic BT SPP (built-in ESP32)

// ════════════════════════════════════════
// COMPILE-TIME CONFIG
// ════════════════════════════════════════
constexpr uint8_t  LED_PIN         = 12;
constexpr uint16_t NUM_LEDS        = 297;
constexpr uint8_t  OLED_ADDR       = 0x3C;
constexpr uint8_t  MPU_ADDR        = 0x69;
constexpr uint8_t  BMP_ADDR        = 0x77;
constexpr uint8_t  OSS             = 1;

constexpr long     GMT_OFFSET      = 19800;   // IST = UTC+5:30
constexpr int      DST_OFFSET      = 0;
constexpr uint32_t WEATHER_INTERVAL= 60000;
constexpr uint32_t TIME_INTERVAL   = 5000;
constexpr uint32_t SENSOR_INTERVAL = 500;
constexpr uint32_t OLED_INTERVAL   = 2000;
constexpr uint32_t PRINT_INTERVAL  = 3000;
constexpr uint32_t WIFIPRINT_INTERVAL = 10000;
constexpr uint32_t APDS_PHASE_MS   = 2000;    // color↔gesture switch time

// ── Credentials (move to secrets.h in production) ──
static const char* WIFI_SSID     = "JAGASURIYA";
static const char* WIFI_PASSWORD = "12345678";
static const char* OWM_API_KEY   = "df6fa81aa0750ec378521712f5c8fa62";
static const char* OWM_CITY      = "Chennai";
static const char* NTP_SERVER    = "pool.ntp.org";
static const char* BT_DEVICE     = "CIRCADIA-BT";   // ← name visible when pairing

// ════════════════════════════════════════
// COLOUR PRESETS  (R, G, B, brightness 0-255, label, Kelvin equiv)
// ════════════════════════════════════════
struct ColorPreset {
  uint8_t     r, g, b;
  uint8_t     brightness;   // 0–255 NeoPixel brightness
  const char* label;
  int         kelvin;
};

static const ColorPreset PRESETS[] = {
  { 255, 197, 143,  160, "WARM",  2700 },   // warm incandescent
  { 100, 150, 255,  200, "COOL",  6000 },   // cool daylight
  { 255, 255, 255,  220, "WHITE", 5500 },   // neutral white
  { 255, 100,   0,  180, "AMBER", 2000 },   // deep amber
};
constexpr uint8_t NUM_PRESETS = 4;

// ════════════════════════════════════════
// OBJECTS
// ════════════════════════════════════════
Adafruit_APDS9960  apds;
Adafruit_NeoPixel  strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306   display(128, 64, &Wire, -1);
BluetoothSerial    SerialBT;                // ← BT serial object

// ════════════════════════════════════════
// MUTEXES
// ════════════════════════════════════════
static SemaphoreHandle_t dataMutex;        // shared structs
static SemaphoreHandle_t wireMutex;        // I²C bus
static SemaphoreHandle_t btMutex;          // SerialBT.print (called from both cores)

// ════════════════════════════════════════
// btPrint — thread-safe BT output
// Mirrors to USB Serial as well
// ════════════════════════════════════════
static void btPrint(const char* msg) {
  Serial.print(msg);
  if (xSemaphoreTake(btMutex, 10) == pdTRUE) {
    if (SerialBT.hasClient()) SerialBT.print(msg);
    xSemaphoreGive(btMutex);
  }
}
static void btPrintln(const char* msg) {
  Serial.println(msg);
  if (xSemaphoreTake(btMutex, 10) == pdTRUE) {
    if (SerialBT.hasClient()) SerialBT.println(msg);
    xSemaphoreGive(btMutex);
  }
}
// printf-style wrapper (128 char limit per call — enough for sensor lines)
static void btPrintf(const char* fmt, ...) {
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  btPrint(buf);
}

// ════════════════════════════════════════
// SHARED STATE
// ════════════════════════════════════════
struct WiFiInfo {
  int    hour=0, minute=0, second=0;
  int    day=0,  month=0,  year=0, wday=0;
  float  temp=0, feels=0, humidity=0, wind=0;
  String sky  = "---";
  String desc = "---";
  bool   timeOK = false;
  bool   wifiOK = false;
} winfo;

// LED live state — written by command parser AND gesture handler
struct LedState {
  uint8_t r=255, g=197, b=143;
  uint8_t brightness=160;        // NeoPixel 0–255
  String  label   = "WARM";
  int     kelvin  = 2700;
  int     userBrightPct = 63;    // 0–100 display percentage
} ledState;

// Sensor readings (Core 1 owns, no mutex needed for writes)
static float ax,ay,az,gx,gy,gz,mpuTemp,tiltX,tiltY;
static int16_t  cal_AC1,cal_AC2,cal_AC3;
static uint16_t cal_AC4,cal_AC5,cal_AC6;
static int16_t  cal_B1,cal_B2,cal_MB,cal_MC,cal_MD;
static float bmpTempC,bmpTempF,pressHpa,pressurePa,pressureMmhg,altitude;
static uint16_t apdsR=0,apdsG=0,apdsB=0,apdsLux=0;
static String   lastGesture = "NONE";

static unsigned long apdsPhaseStart = 0;
static bool          apdsColorPhase = true;
static uint8_t       oledPage       = 0;

// Day / month name tables
static const char* WEEKDAYS[]  = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
static const char* MONTHS[]    = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

// ════════════════════════════════════════
// LED HELPERS
// ════════════════════════════════════════
static void applyLED() {
  // Apply current ledState to strip (call from any core — strip.show() is not mutex-guarded
  // but sensorTask is the only writer; command parser sets ledState under dataMutex,
  // and sensorTask calls applyLED each loop, so one-tick latency max)
  strip.setBrightness(ledState.brightness);
  for (int i = 0; i < NUM_LEDS; i++)
    strip.setPixelColor(i, strip.Color(ledState.r, ledState.g, ledState.b));
  strip.show();
}

// Apply a named preset + optional brightness override (0 = keep preset default)
static void applyPreset(const char* name, int brightPct) {
  for (uint8_t i = 0; i < NUM_PRESETS; i++) {
    if (strcasecmp(PRESETS[i].label, name) == 0) {
      if (xSemaphoreTake(dataMutex, 50) == pdTRUE) {
        ledState.r      = PRESETS[i].r;
        ledState.g      = PRESETS[i].g;
        ledState.b      = PRESETS[i].b;
        ledState.label  = PRESETS[i].label;
        ledState.kelvin = PRESETS[i].kelvin;
        // brightness: use override if given, else preset default
        if (brightPct > 0) {
          ledState.userBrightPct = constrain(brightPct, 1, 100);
          ledState.brightness    = (uint8_t)(brightPct * 255 / 100);
        } else {
          ledState.brightness    = PRESETS[i].brightness;
          ledState.userBrightPct = (int)(PRESETS[i].brightness * 100 / 255);
        }
        xSemaphoreGive(dataMutex);
      }
      return;
    }
  }
}

static void applyBrightness(int pct) {
  pct = constrain(pct, 1, 100);
  if (xSemaphoreTake(dataMutex, 50) == pdTRUE) {
    ledState.userBrightPct = pct;
    ledState.brightness    = (uint8_t)(pct * 255 / 100);
    xSemaphoreGive(dataMutex);
  }
}

// ════════════════════════════════════════
// BLUETOOTH COMMAND PARSER
// Called from Core 0 loop — reads one complete line per call
// ════════════════════════════════════════
static String btBuffer = "";

static void btHelp() {
  btPrintln("┌─ CIRCADIA-BT COMMANDS ─────────────────┐");
  btPrintln("│ COLOR:WARM          Warm white  2700K   │");
  btPrintln("│ COLOR:COOL          Cool white  6000K   │");
  btPrintln("│ COLOR:WHITE         Bright white 5500K  │");
  btPrintln("│ COLOR:AMBER         Deep amber  2000K   │");
  btPrintln("│ BRIGHT:0-100        Set brightness %    │");
  btPrintln("│ COLOR:WARM,75       Color + brightness  │");
  btPrintln("│ STATUS              Sensor snapshot     │");
  btPrintln("│ HELP                This list           │");
  btPrintln("└────────────────────────────────────────┘");
}

static void btStatus() {
  LedState ls;
  WiFiInfo w;
  if (xSemaphoreTake(dataMutex, 50) == pdTRUE) { ls=ledState; w=winfo; xSemaphoreGive(dataMutex); }
  btPrintln("─── STATUS SNAPSHOT ───────────────────────");
  btPrintf("  LED     : %s  %dK  Bright=%d%%\n", ls.label.c_str(), ls.kelvin, ls.userBrightPct);
  btPrintf("  Gesture : %s\n", lastGesture.c_str());
  btPrintf("  BMP Temp: %.1f C   Press: %.1f hPa\n", bmpTempC, pressHpa);
  btPrintf("  Tilt    : X=%.1f  Y=%.1f  MPU T=%.1fC\n", tiltX, tiltY, mpuTemp);
  btPrintf("  APDS Lux: %d  R=%d G=%d B=%d\n", apdsLux, apdsR, apdsG, apdsB);
  if (w.timeOK)
    btPrintf("  Time    : %02d:%02d:%02d  %02d/%02d/%d\n",
             w.hour,w.minute,w.second, w.day,w.month,w.year);
  if (w.sky != "---")
    btPrintf("  Weather : %.1fC  %s\n", w.temp, w.sky.c_str());
  btPrintln("───────────────────────────────────────────");
}

// Parse one complete command line (no newline)
static void parseCommand(String& cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "HELP") {
    btHelp();
    return;
  }
  if (cmd == "STATUS") {
    btStatus();
    return;
  }

  // COLOR:WARM  or  COLOR:WARM,75
  if (cmd.startsWith("COLOR:")) {
    String arg = cmd.substring(6);   // "WARM" or "WARM,75"
    int    comma = arg.indexOf(',');
    String colorName;
    int    bright = 0;
    if (comma >= 0) {
      colorName = arg.substring(0, comma);
      bright    = arg.substring(comma + 1).toInt();
    } else {
      colorName = arg;
    }
    applyPreset(colorName.c_str(), bright);

    LedState ls;
    if (xSemaphoreTake(dataMutex, 20) == pdTRUE) { ls=ledState; xSemaphoreGive(dataMutex); }
    btPrintf("✓ Color set: %s  %dK  Bright=%d%%\n",
             ls.label.c_str(), ls.kelvin, ls.userBrightPct);
    return;
  }

  // BRIGHT:75
  if (cmd.startsWith("BRIGHT:")) {
    int pct = cmd.substring(7).toInt();
    if (pct < 1 || pct > 100) {
      btPrintln("✗ BRIGHT must be 1–100  (e.g. BRIGHT:60)");
      return;
    }
    applyBrightness(pct);
    btPrintf("✓ Brightness set: %d%%\n", pct);
    return;
  }

  btPrintf("✗ Unknown command: %s  (type HELP)\n", cmd.c_str());
}

// Called every Core 0 loop — drains BT RX buffer one char at a time
static void handleBTInput() {
  while (SerialBT.available()) {
    char c = (char)SerialBT.read();
    if (c == '\n' || c == '\r') {
      if (btBuffer.length() > 0) {
        parseCommand(btBuffer);
        btBuffer = "";
      }
    } else {
      if (btBuffer.length() < 64) btBuffer += c;  // guard against overflow
    }
  }
}

// ════════════════════════════════════════
// I²C helpers (wire-mutex protected)
// ════════════════════════════════════════
static void i2cWrite(uint8_t addr, uint8_t reg, uint8_t val) {
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  Wire.beginTransmission(addr); Wire.write(reg); Wire.write(val);
  Wire.endTransmission(true);
  xSemaphoreGive(wireMutex);
}
static uint8_t i2cRead8(uint8_t addr, uint8_t reg) {
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  Wire.beginTransmission(addr); Wire.write(reg); Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)addr,(uint8_t)1,(uint8_t)true);
  uint8_t v = Wire.available() ? Wire.read() : 0xFF;
  xSemaphoreGive(wireMutex);
  return v;
}
static void i2cReadBurst(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {
  xSemaphoreTake(wireMutex, portMAX_DELAY);
  Wire.beginTransmission(addr); Wire.write(reg); Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)addr,(uint8_t)len,(uint8_t)true);
  for (uint8_t i=0;i<len;i++) buf[i]=Wire.available()?Wire.read():0;
  xSemaphoreGive(wireMutex);
}
static int16_t  i2cReadS16(uint8_t a,uint8_t r){return(int16_t)((i2cRead8(a,r)<<8)|i2cRead8(a,r+1));}
static uint16_t i2cReadU16(uint8_t a,uint8_t r){return(uint16_t)((i2cRead8(a,r)<<8)|i2cRead8(a,r+1));}

// ════════════════════════════════════════
// MPU6050
// ════════════════════════════════════════
static bool mpuInit() {
  i2cWrite(MPU_ADDR,0x6B,0x80); delay(200);
  i2cWrite(MPU_ADDR,0x6B,0x01); delay(100);
  uint8_t who=i2cRead8(MPU_ADDR,0x75);
  Serial.printf("[MPU] WHO_AM_I: 0x%02X\n",who);
  if(who!=0x68&&who!=0x70&&who!=0x98) return false;
  i2cWrite(MPU_ADDR,0x68,0x07); delay(50);
  i2cWrite(MPU_ADDR,0x6A,0x00);
  i2cWrite(MPU_ADDR,0x19,0x07);
  i2cWrite(MPU_ADDR,0x1A,0x03);
  i2cWrite(MPU_ADDR,0x1B,0x00);
  i2cWrite(MPU_ADDR,0x1C,0x00);
  i2cWrite(MPU_ADDR,0x38,0x00);
  delay(50);
  uint8_t dummy[14];
  for(int i=0;i<5;i++){i2cReadBurst(MPU_ADDR,0x3B,dummy,14);delay(20);}
  return true;
}
static void mpuReadAll() {
  uint8_t buf[14];
  i2cReadBurst(MPU_ADDR,0x3B,buf,14);
  ax=((int16_t)(buf[0]<<8|buf[1]))/16384.0f;
  ay=((int16_t)(buf[2]<<8|buf[3]))/16384.0f;
  az=((int16_t)(buf[4]<<8|buf[5]))/16384.0f;
  mpuTemp=((int16_t)(buf[6]<<8|buf[7]))/340.0f+36.53f;
  gx=((int16_t)(buf[8]<<8|buf[9]))/131.0f;
  gy=((int16_t)(buf[10]<<8|buf[11]))/131.0f;
  gz=((int16_t)(buf[12]<<8|buf[13]))/131.0f;
  tiltX=atan2f(ax,sqrtf(ay*ay+az*az))*180.0f/PI;
  tiltY=atan2f(ay,sqrtf(ax*ax+az*az))*180.0f/PI;
}

// ════════════════════════════════════════
// BMP180
// ════════════════════════════════════════
static void bmpReadCalibration(){
  cal_AC1=i2cReadS16(BMP_ADDR,0xAA); cal_AC2=i2cReadS16(BMP_ADDR,0xAC);
  cal_AC3=i2cReadS16(BMP_ADDR,0xAE); cal_AC4=i2cReadU16(BMP_ADDR,0xB0);
  cal_AC5=i2cReadU16(BMP_ADDR,0xB2); cal_AC6=i2cReadU16(BMP_ADDR,0xB4);
  cal_B1 =i2cReadS16(BMP_ADDR,0xB6); cal_B2 =i2cReadS16(BMP_ADDR,0xB8);
  cal_MB =i2cReadS16(BMP_ADDR,0xBA); cal_MC =i2cReadS16(BMP_ADDR,0xBC);
  cal_MD =i2cReadS16(BMP_ADDR,0xBE);
  Serial.printf("[BMP] Calib OK: AC1=%d AC5=%u AC6=%u\n",cal_AC1,cal_AC5,cal_AC6);
}
static void bmpReadAll(){
  i2cWrite(BMP_ADDR,0xF4,0x2E); delay(5);
  int32_t UT=(int32_t)i2cReadS16(BMP_ADDR,0xF6);
  i2cWrite(BMP_ADDR,0xF4,(uint8_t)(0x34+(OSS<<6))); delay(8);
  // read 3 raw pressure bytes
  xSemaphoreTake(wireMutex,portMAX_DELAY);
  Wire.beginTransmission(BMP_ADDR); Wire.write(0xF6); Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)BMP_ADDR,(uint8_t)3,(uint8_t)true);
  uint8_t msb=Wire.available()?Wire.read():0;
  uint8_t lsb=Wire.available()?Wire.read():0;
  uint8_t xlsb=Wire.available()?Wire.read():0;
  xSemaphoreGive(wireMutex);
  int32_t UP=(int32_t)(((uint32_t)msb<<16)|((uint32_t)lsb<<8)|(uint32_t)xlsb)>>(8-OSS);
  int32_t X1=((UT-(int32_t)cal_AC6)*(int32_t)cal_AC5)>>15;
  int32_t X2=((int32_t)cal_MC<<11)/(X1+(int32_t)cal_MD);
  int32_t B5=X1+X2;
  bmpTempC=((B5+8)>>4)/10.0f; bmpTempF=bmpTempC*9.0f/5.0f+32.0f;
  int32_t B6=B5-4000;
  X1=((int32_t)cal_B2*((B6*B6)>>12))>>11;
  X2=((int32_t)cal_AC2*B6)>>11;
  int32_t X3=X1+X2;
  int32_t B3=((((int32_t)cal_AC1*4+X3)<<OSS)+2)>>2;
  X1=((int32_t)cal_AC3*B6)>>13;
  X2=((int32_t)cal_B1*((B6*B6)>>12))>>16;
  X3=((X1+X2)+2)>>2;
  uint32_t B4=((uint32_t)cal_AC4*(uint32_t)(X3+32768))>>15;
  uint32_t B7=((uint32_t)UP-B3)*(50000UL>>OSS);
  int32_t  p=(B7<0x80000000UL)?(int32_t)((B7*2)/B4):(int32_t)((B7/B4)*2);
  X1=(p>>8)*(p>>8); X1=(X1*3038)>>16; X2=(-7357*p)>>16;
  p=p+((X1+X2+3791)>>4);
  pressurePa=(float)p; pressHpa=p/100.0f;
  pressureMmhg=p*0.00750062f;
  altitude=44330.0f*(1.0f-powf(pressHpa/1013.25f,1.0f/5.255f));
}

// ════════════════════════════════════════
// APDS9960 — alternating color/gesture phases
// ════════════════════════════════════════
static void apdsSetPhase(bool colorMode){
  apdsColorPhase=colorMode; apdsPhaseStart=millis();
  if(colorMode){apds.enableGesture(false);apds.enableColor(true); apds.setADCGain(APDS9960_AGAIN_16X);}
  else         {apds.enableColor(false); apds.enableGesture(true);}
}
static void apdsHandle(){
  if(millis()-apdsPhaseStart>APDS_PHASE_MS) apdsSetPhase(!apdsColorPhase);

  if(apdsColorPhase&&apds.colorDataReady()){
    uint16_t r,g,b,c; apds.getColorData(&r,&g,&b,&c);
    apdsR=r; apdsG=g; apdsB=b; apdsLux=c;
  }
  if(!apdsColorPhase){
    uint8_t gest=apds.readGesture();
    const char* gname=nullptr;
    const char* preset=nullptr;
    if     (gest==APDS9960_UP)   {gname="UP";    preset="COOL"; }
    else if(gest==APDS9960_DOWN) {gname="DOWN";  preset="WARM"; }
    else if(gest==APDS9960_LEFT) {gname="LEFT";  preset="AMBER";}
    else if(gest==APDS9960_RIGHT){gname="RIGHT"; preset="WHITE";}
    if(gname){
      lastGesture=gname;
      applyPreset(preset, 0);
      btPrintf("[APDS] Gesture %s → %s\n", gname, preset);
    }
  }
}

// ════════════════════════════════════════
// OLED — 4 screens
// ════════════════════════════════════════
static void updateOLED(){
  WiFiInfo w;
  LedState ls;
  if(xSemaphoreTake(dataMutex,50)==pdTRUE){w=winfo;ls=ledState;xSemaphoreGive(dataMutex);}

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  switch(oledPage){

    case 0:{   // ── Clock + Date + Weather ──
      display.setTextSize(2);
      display.setCursor(10,2);
      if(w.timeOK){char t[6];snprintf(t,6,"%02d:%02d",w.hour,w.minute);display.print(t);}
      else         display.print("--:--");
      display.setTextSize(1);
      display.setCursor(98,8);
      if(w.timeOK){char s[3];snprintf(s,3,"%02d",w.second);display.print(s);}
      if(w.wifiOK) display.fillCircle(122,3,3,SSD1306_WHITE);
      else         display.drawCircle(122,3,3,SSD1306_WHITE);
      // BT dot (bottom-right) — filled when client connected
      if(SerialBT.hasClient()) display.fillRect(121,57,6,6,SSD1306_WHITE);
      else                     display.drawRect(121,57,6,6,SSD1306_WHITE);
      display.drawFastHLine(0,26,128,SSD1306_WHITE);
      display.setCursor(0,31);
      if(w.timeOK){
        char d[14];
        snprintf(d,14,"%02d %s %d",w.day,MONTHS[w.month-1],w.year%100);
        display.print(d);
      } else display.print("Syncing time...");
      display.setCursor(0,46);
      if(w.sky!="---"){
        char wl[22];
        snprintf(wl,22,"%.0fC %.7s %.6s",w.temp,w.sky.c_str(),OWM_CITY);
        display.print(wl);
      } else display.print("Weather fetching...");
      break;
    }

    case 1:{   // ── Environment ──
      display.setTextSize(1);
      display.setCursor(0,0); display.print("Environment");
      display.drawFastHLine(0,10,128,SSD1306_WHITE);
      display.setTextSize(2);
      display.setCursor(0,14);
      char tc[8]; snprintf(tc,8,"%.1fC",bmpTempC); display.print(tc);
      display.setTextSize(1);
      display.setCursor(0,36);
      char pr[20]; snprintf(pr,20,"Press: %.1f hPa",pressHpa); display.print(pr);
      display.setCursor(0,50);
      char al[16]; snprintf(al,16,"Alt:   %.0f m",altitude); display.print(al);
      break;
    }

    case 2:{   // ── Motion ──
      display.setTextSize(1);
      display.setCursor(0,0); display.print("Motion");
      display.drawFastHLine(0,10,128,SSD1306_WHITE);
      char tx[16]; snprintf(tx,16,"Tilt X: %+.1f",tiltX);
      display.setCursor(0,15); display.print(tx);
      char ty[16]; snprintf(ty,16,"Tilt Y: %+.1f",tiltY);
      display.setCursor(0,28); display.print(ty);
      char mt[16]; snprintf(mt,16,"Die T:  %.1fC",mpuTemp);
      display.setCursor(0,41); display.print(mt);
      char gs[20]; snprintf(gs,20,"Gest:   %s",lastGesture.c_str());
      display.setCursor(0,54); display.print(gs);
      break;
    }

    case 3:{   // ── LED / BT status ──
      display.setTextSize(1);
      display.setCursor(0,0); display.print("Lighting");
      // BT connection label top-right
      display.setCursor(80,0);
      display.print(SerialBT.hasClient()?"BT:ON":"BT:---");
      display.drawFastHLine(0,10,128,SSD1306_WHITE);
      display.setTextSize(2);
      display.setCursor(0,14); display.print(ls.label.c_str());
      display.setTextSize(1);
      char kl[12]; snprintf(kl,12,"%d K",ls.kelvin);
      display.setCursor(80,18); display.print(kl);
      char rgb[20]; snprintf(rgb,20,"R:%d G:%d B:%d",ls.r,ls.g,ls.b);
      display.setCursor(0,38); display.print(rgb);
      display.setCursor(0,52); display.print("Bright:");
      int barW=(ls.brightness*70)/255;
      display.drawRect(50,51,70,7,SSD1306_WHITE);
      display.fillRect(50,51,barW,7,SSD1306_WHITE);
      break;
    }
  }
  display.display();
  oledPage=(oledPage+1)%4;
}

// ════════════════════════════════════════
// SERIAL PRINT helpers (mirrored to BT)
// ════════════════════════════════════════
static const char* timePeriod(int h){
  if(h>=6 &&h<9)  return "Morning";
  if(h>=9 &&h<12) return "Late Morning";
  if(h>=12&&h<14) return "Noon";
  if(h>=14&&h<17) return "Afternoon";
  if(h>=17&&h<20) return "Evening";
  if(h>=20&&h<23) return "Night";
  return "Late Night";
}

static void printSensors(){
  LedState ls;
  if(xSemaphoreTake(dataMutex,10)==pdTRUE){ls=ledState;xSemaphoreGive(dataMutex);}
  btPrintln("════════════════════════════════════");
  btPrintf("[MPU]  A:%.2fg %.2fg %.2fg  T:%.1fC\n",ax,ay,az,mpuTemp);
  btPrintf("       Tilt X=%.1f  Y=%.1f\n",tiltX,tiltY);
  btPrintf("[BMP]  %.1fC  %.1f hPa  %.0fm\n",bmpTempC,pressHpa,altitude);
  btPrintf("[APDS] Lux:%d R:%d G:%d B:%d  Gest:%s\n",apdsLux,apdsR,apdsG,apdsB,lastGesture.c_str());
  btPrintf("[LED]  %s %dK Bright=%d%%  BT:%s\n",
           ls.label.c_str(),ls.kelvin,ls.userBrightPct,
           SerialBT.hasClient()?"connected":"---");
  btPrintln("════════════════════════════════════");
}

static void printWiFiData(){
  WiFiInfo w;
  if(xSemaphoreTake(dataMutex,50)==pdTRUE){w=winfo;xSemaphoreGive(dataMutex);}
  btPrintln("╔══ DATE / TIME / WEATHER ══════════╗");
  if(w.timeOK)
    btPrintf("  %s  %02d %s %d  %02d:%02d:%02d  %s\n",
             WEEKDAYS[w.wday],w.day,MONTHS[w.month-1],w.year,
             w.hour,w.minute,w.second,timePeriod(w.hour));
  if(w.sky!="---")
    btPrintf("  %s  %.1fC (feels %.1fC)  Hum=%.0f%%  Wind=%.1fm/s\n",
             w.sky.c_str(),w.temp,w.feels,w.humidity,w.wind);
  btPrintln("╚═══════════════════════════════════╝");
}

// ════════════════════════════════════════
// CORE 0 — WiFi + NTP + Weather + BT RX
// ════════════════════════════════════════
void wifiTask(void* param){
  Serial.println("[Core0] WiFi task started");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int t=0;
  while(WiFi.status()!=WL_CONNECTED&&t<30){delay(500);Serial.print('.');t++;}
  Serial.println();

  if(WiFi.status()==WL_CONNECTED){
    Serial.printf("[Core0] WiFi connected: %s\n",WiFi.localIP().toString().c_str());
    if(xSemaphoreTake(dataMutex,100)==pdTRUE){winfo.wifiOK=true;xSemaphoreGive(dataMutex);}
    configTime(GMT_OFFSET,DST_OFFSET,NTP_SERVER);
    delay(1000);
  } else {
    Serial.println("[Core0] WiFi FAILED — offline mode");
  }

  unsigned long lastTime=0, lastWeather=0;

  for(;;){
    unsigned long now=millis();

    // ── Read BT commands ────────────────────────────────────────
    handleBTInput();

    // ── NTP time every 5 s ─────────────────────────────────────
    if(now-lastTime>TIME_INTERVAL){
      lastTime=now;
      struct tm ti;
      if(getLocalTime(&ti)){
        if(xSemaphoreTake(dataMutex,100)==pdTRUE){
          winfo.hour=ti.tm_hour; winfo.minute=ti.tm_min; winfo.second=ti.tm_sec;
          winfo.day=ti.tm_mday;  winfo.month=ti.tm_mon+1;
          winfo.year=ti.tm_year+1900; winfo.wday=ti.tm_wday; winfo.timeOK=true;
          xSemaphoreGive(dataMutex);
        }
      }
    }

    // ── Weather every 60 s ─────────────────────────────────────
    if(now-lastWeather>WEATHER_INTERVAL){
      lastWeather=now;
      if(WiFi.status()==WL_CONNECTED){
        HTTPClient http;
        String url="http://api.openweathermap.org/data/2.5/weather?q="+
                   String(OWM_CITY)+"&appid="+String(OWM_API_KEY)+"&units=metric";
        http.begin(url); http.setTimeout(8000);
        int code=http.GET();
        if(code==200){
          StaticJsonDocument<1024> doc;
          DeserializationError err=deserializeJson(doc,http.getString());
          if(!err){
            if(xSemaphoreTake(dataMutex,100)==pdTRUE){
              winfo.temp    =doc["main"]["temp"];
              winfo.feels   =doc["main"]["feels_like"];
              winfo.humidity=doc["main"]["humidity"];
              winfo.wind    =doc["wind"]["speed"];
              winfo.sky     =doc["weather"][0]["main"].as<String>();
              winfo.desc    =doc["weather"][0]["description"].as<String>();
              xSemaphoreGive(dataMutex);
            }
            btPrintln("[Core0] Weather updated");
          }
        } else {
          btPrintf("[Core0] Weather error: %d\n",code);
        }
        http.end();
      }
    }

    delay(20);   // tight loop for responsive BT input
  }
}

// ════════════════════════════════════════
// CORE 1 — Sensors + LED + OLED + Print
// ════════════════════════════════════════
void sensorTask(void* param){
  Serial.println("[Core1] Sensor task started");
  unsigned long prevSensor=0,prevOLED=0,prevPrint=0,prevWifi=0;

  for(;;){
    unsigned long now=millis();

    apdsHandle();

    if(now-prevSensor>=SENSOR_INTERVAL){
      prevSensor=now;
      mpuReadAll();
      bmpReadAll();
    }

    // Always apply LED (picks up BT command changes instantly)
    applyLED();

    if(now-prevOLED>=OLED_INTERVAL){
      prevOLED=now;
      updateOLED();
    }
    if(now-prevPrint>=PRINT_INTERVAL){
      prevPrint=now;
      printSensors();
    }
    if(now-prevWifi>=WIFIPRINT_INTERVAL){
      prevWifi=now;
      printWiFiData();
    }

    delay(5);
  }
}

// ════════════════════════════════════════
// SETUP
// ════════════════════════════════════════
void setup(){
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21,22);
  Wire.setClock(400000);

  dataMutex = xSemaphoreCreateMutex();
  wireMutex = xSemaphoreCreateMutex();
  btMutex   = xSemaphoreCreateMutex();

  // ── Bluetooth ──────────────────────────────────────────────
  SerialBT.begin(BT_DEVICE);
  Serial.printf("[BT] Started as \"%s\" — pair and connect\n", BT_DEVICE);

  // ── LED ────────────────────────────────────────────────────
  strip.begin();
  strip.setBrightness(ledState.brightness);
  for(int i=0;i<NUM_LEDS;i++)
    strip.setPixelColor(i,strip.Color(ledState.r,ledState.g,ledState.b));
  strip.show();

  // ── OLED ───────────────────────────────────────────────────
  if(!display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR)){
    Serial.println("[OLED] FAIL"); while(1);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(16,14); display.print("CIRCADIA-SENSE");
  display.setCursor(16,28); display.print("CIT  Team Rival");
  display.setCursor(16,44); display.print("Initialising...");
  display.display();
  delay(2000);

  // ── MPU6050 ────────────────────────────────────────────────
  bool mpuOK=mpuInit();
  Serial.println(mpuOK?"[MPU] OK":"[MPU] FAIL");

  // ── BMP180 ─────────────────────────────────────────────────
  uint8_t bmpID=i2cRead8(BMP_ADDR,0xD0);
  bool bmpOK=(bmpID==0x55);
  Serial.printf("[BMP] ID:0x%02X %s\n",bmpID,bmpOK?"OK":"FAIL");
  if(bmpOK) bmpReadCalibration();

  // ── APDS9960 ───────────────────────────────────────────────
  bool apdsOK=apds.begin();
  Serial.println(apdsOK?"[APDS] OK":"[APDS] FAIL");
  if(!apdsOK){ Serial.println("[APDS] FAIL — halting"); while(1); }
  apds.enableProximity(true);
  apdsSetPhase(true);   // start in color phase

  // ── Status screen ──────────────────────────────────────────
  display.clearDisplay();
  display.setCursor(0, 0); display.print(mpuOK ?"MPU6050 : OK"  :"MPU6050 : FAIL");
  display.setCursor(0,12); display.print(bmpOK ?"BMP180  : OK"  :"BMP180  : FAIL");
  display.setCursor(0,24); display.print(apdsOK?"APDS9960: OK"  :"APDS9960: FAIL");
  display.setCursor(0,36); display.print("WS2812B : OK 297LED");
  display.setCursor(0,48); display.print("BT: CIRCADIA-BT");
  display.display();
  delay(2000);

  btHelp();   // print command list to USB Serial at boot

  xTaskCreatePinnedToCore(wifiTask,  "WiFiTask",  12288,NULL,1,NULL,0);
  xTaskCreatePinnedToCore(sensorTask,"SensorTask", 6144,NULL,1,NULL,1);
}

void loop(){ vTaskDelay(portMAX_DELAY); }
