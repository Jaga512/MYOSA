# MYOSA
# Acknowledgements

We sincerely thank the IEEE MYOSA organizing committee for providing a platform that encourages innovation in embedded systems, IoT, healthcare, and sustainable technologies. We also express our gratitude to Chennai Institute of Technology for supporting our research and development efforts. Special thanks to our faculty mentor for guiding us throughout the development of CIRCADIA-SENSE.

# Overview

CIRCADIA-SENSE is an intelligent adaptive lighting system designed to improve human health and energy efficiency through biologically optimized smart lighting. Traditional smart lighting systems mainly focus on automation and power savings, but they often ignore the effects of artificial lighting on human circadian rhythms. Improper lighting can lead to sleep disorders, reduced productivity, eye strain, fatigue, and long-term health issues.

Our system uses the MYOSA platform along with multiple environmental and motion sensors to create a context-aware lighting ecosystem. The system continuously monitors ambient light, movement, occupancy, environmental conditions, and user activities to dynamically adjust lighting intensity and color temperature.

The lighting environment changes automatically according to activities such as reading, working, relaxing, or sleeping. Cool bright lighting is used during work hours to improve alertness, while warm lighting is used during relaxation periods to support healthy sleep cycles and reduce eye strain.

The project combines IoT, embedded systems, AI-based activity recognition, and energy optimization techniques to create a healthier and smarter indoor environment.

## Key Features

- Circadian rhythm optimization  
- Multi-sensor context awareness  
- Adaptive lighting control  
- Gesture-based interaction  
- Energy-efficient smart lighting  
- Wireless monitoring and control  
- AI-based activity recognition  
- Multi-zone lighting management  

# Demo / Examples

## Images

### System Architecture Diagram
- Complete workflow of sensors, MYOSA board, cloud communication, and adaptive lighting control system.

### Smart Room Prototype
The prototype includes multiple intelligent lighting zones:
- Workspace Area  
- Reading Corner  
- Relaxation Zone  
- Transition Space  

### Dashboard Interface
The dashboard displays:
- Ambient light data  
- Occupancy detection  
- Energy consumption  
- Current lighting mode  
- Color temperature visualization  

### Mobile Application
- Remote monitoring and lighting control  
- Manual brightness adjustment  
- Mode selection and scheduling  

### OLED Monitoring Display
- Real-time sensor readings  
- System status updates  
- Active lighting mode display  

# Videos

https://github.com/user-attachments/assets/0b22f1b0-9e0b-4e76-b8da-f13e3017a899


https://github.com/user-attachments/assets/c1df97db-31fe-4a79-8810-1b82975b1a10



https://github.com/user-attachments/assets/de0d2bd2-e9d6-4870-a21b-eecabc2bfe61


The demonstration videos include:

- Complete hardware setup explanation  
- Real-time adaptive lighting demonstration  
- Gesture-based controls  
- Occupancy detection demo  
- Energy optimization demonstration  
- Mobile dashboard monitoring  
- Comparison between conventional lighting and CIRCADIA-SENSE adaptive lighting  
- Multi-zone smart lighting behavior  
- Real-time sensor monitoring and OLED display visualization

---

# Features

## 1. Circadian Rhythm Optimization
Automatically adjusts lighting based on human biological cycles using color temperatures between **2000K–6500K** for better focus, comfort, and sleep support.

## 2. Multi-Sensor Context Awareness
Uses multiple sensors for intelligent environment monitoring:
- APDS9960 → Ambient light & gesture sensing  
- MPU6050 → Motion & occupancy detection  
- BMP180 → Temperature monitoring  
- OLED Display → Real-time system data  
- Buzzer → Notifications & alerts  

## 3. Adaptive Activity Recognition
Automatically switches lighting modes:
- Reading Mode  
- Work Mode  
- Relaxation Mode  
- Sleep Preparation Mode  

## 4. Intelligent Energy Optimization
Optimizes power usage using:
- Automatic dimming  
- Occupancy-based activation  
- Smart brightness adjustment  
- AI-based energy optimization  

Reduces electricity consumption by approximately **30–40%**.

## 5. Wireless Smart Connectivity
Supports:
- Wi-Fi monitoring  
- Bluetooth connectivity  
- Cloud synchronization  
- Smart home integration  
- OTA firmware updates  

## 6. Machine Learning-Based Automation
Analyzes user behavior and lighting preferences for:
- Personalized lighting  
- Better automation accuracy  
- Reduced energy wastage  

## 7. Multi-Zone Adaptive Lighting
Supports independent lighting control for:
- Workspace Area  
- Reading Corner  
- Relaxation Zone  
- Transition Space

  ---
# Usage Instructions

## 1. Hardware Setup
Connect the following components to the MYOSA board:
- APDS9960 Sensor  
- MPU6050 Sensor  
- BMP180 Sensor  
- OLED Display  
- RGB LED Strip  
- MOSFET Driver Modules  
- RTC Module  
- Power Supply  

## 2. Install Required Software
- Arduino IDE  
- ESP32 Board Drivers  
- Python 3.x  
- Flask Framework  

## 3. Install Required Libraries

### Arduino Libraries
- Adafruit_APDS9960  
- Adafruit_MPU6050  
- Adafruit_BMP085  
- FastLED  
- WiFi  
- BluetoothSerial  

### Python Libraries
- Flask  
- NumPy  
- Pandas  
- OpenCV  

## 4. Upload Firmware
Upload the firmware to the ESP32-based MYOSA board using Arduino IDE.

## 5. Configure Wi-Fi
Update Wi-Fi credentials for remote monitoring and cloud synchronization.

## 6. Start the System
Power on the system and observe adaptive lighting behavior based on user activity and environmental conditions.

---

# Tech Stack

## Hardware
- MYOSA ESP32 Development Board  
- APDS9960 Sensor  
- MPU6050 Sensor  
- BMP180 Sensor  
- OLED Display  
- RGB LED Strip  
- MOSFET Modules  

## Software
- Python  
- Embedded C++  
- Arduino IDE  
- Flask  
- Machine Learning Algorithms  

## Technologies
- Sensor Fusion  
- AI-Based Activity Recognition  
- Smart Lighting Automation  
- Cloud Connectivity  
- Wireless Communication  

---

# Requirements / Installation

## Hardware Requirements
- MYOSA Board  
- RGB LED Strips  
- Sensors  
- RTC Module  
- Power Supply  
- Connecting Wires  
- Breadboard / PCB  

## Software Requirements
- Python 3.x  
- Arduino IDE  
- ESP32 Drivers  
- Flask  
- NumPy  
- Pandas  
- OpenCV  

## Installation Command

```bash
pip install flask numpy pandas opencv-python
