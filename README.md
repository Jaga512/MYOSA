<div align="center">

# RIVAL

**Intelligent Adaptive Lighting System**

*Biologically Optimized · Context Aware · Energy Efficient*

---

[![Platform](https://img.shields.io/badge/Platform-MYOSA%20ESP32-2563EB?style=for-the-badge&logo=espressif&logoColor=white)](https://github.com)
[![License](https://img.shields.io/badge/License-Open%20Source-10B981?style=for-the-badge)](https://github.com)
[![IEEE](https://img.shields.io/badge/IEEE-MYOSA-F59E0B?style=for-the-badge)](https://github.com)
[![Institution](https://img.shields.io/badge/Chennai%20Institute%20of%20Technology-8B5CF6?style=for-the-badge)](https://github.com)

</div>

---

## Overview

CIRCADIA-SENSE is an intelligent adaptive lighting system designed to improve human health and energy efficiency through biologically optimized smart lighting. Traditional smart lighting systems mainly focus on automation and power savings, but they often ignore the effects of artificial lighting on human circadian rhythms. Improper lighting can lead to sleep disorders, reduced productivity, eye strain, fatigue, and long-term health issues.

The system uses the MYOSA platform along with multiple environmental and motion sensors to create a context-aware lighting ecosystem. It continuously monitors ambient light, movement, occupancy, environmental conditions, and user activities to dynamically adjust lighting intensity and color temperature.

The lighting environment changes automatically according to activities such as reading, working, relaxing, or sleeping. Cool bright lighting is used during work hours to improve alertness, while warm lighting is used during relaxation periods to support healthy sleep cycles and reduce eye strain.

The project combines IoT, embedded systems, AI-based activity recognition, and energy optimization techniques to create a healthier and smarter indoor environment.

---

## Key Features

- Circadian rhythm optimization
- Multi-sensor context awareness
- Adaptive lighting control
- Gesture-based interaction
- Energy-efficient smart lighting
- Wireless monitoring and control
- AI-based activity recognition
- Multi-zone lighting management

---

## Features

### 1. Circadian Rhythm Optimization

Automatically adjusts lighting based on human biological cycles using color temperatures between **2000K – 6500K** for better focus, comfort, and sleep support.

### 2. Multi-Sensor Context Awareness

Uses multiple sensors for intelligent environment monitoring:

| Sensor | Function |
|---|---|
| APDS9960 | Ambient light and gesture sensing |
| MPU6050 | Motion and occupancy detection |
| BMP180 | Temperature monitoring |
| OLED Display | Real-time system data |
| Buzzer | Notifications and alerts |

### 3. Adaptive Activity Recognition

Automatically switches lighting modes based on detected activity:

| Mode | Lighting Behavior |
|---|---|
| Reading Mode | Warm, focused task lighting |
| Work Mode | Bright, high color-temperature alertness |
| Relaxation Mode | Soft, dimmed warm ambient |
| Sleep Preparation Mode | Deep amber, minimal blue light |

### 4. Intelligent Energy Optimization

Optimizes power usage through automatic dimming, occupancy-based activation, smart brightness adjustment, and AI-based energy optimization. Reduces electricity consumption by approximately **30 – 40%**.

### 5. Wireless Smart Connectivity

| Protocol | Use |
|---|---|
| Wi-Fi | Remote monitoring |
| Bluetooth | Local device control |
| Cloud Sync | Preference storage and analytics |
| OTA Updates | Seamless firmware upgrades |

### 6. Machine Learning-Based Automation

Analyzes user behavior and lighting preferences for personalized lighting, better automation accuracy, and reduced energy wastage.

### 7. Multi-Zone Adaptive Lighting

Supports independent lighting control across four zones:

```
Workspace Area    ·    Reading Corner
Relaxation Zone   ·    Transition Space
```

---
## Images
<img width="576" height="315" alt="Image _1" src="https://github.com/user-attachments/assets/43121214-0888-41fb-952d-6d692caa8ae5" />
<img width="720" height="419" alt="Image_2" src="https://github.com/user-attachments/assets/4ede1707-ccfe-40d1-a509-45caec49a812" />

---

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

## Tech Stack

**Hardware**

```
MYOSA ESP32 Board · APDS9960 · MPU6050 · BMP180
OLED Display · RGB LED Strip · MOSFET Modules · RTC Module
```

**Software**

```
Embedded C++ (Arduino IDE) · Python 3.x · Flask
```

**Data and ML**

```
NumPy · Pandas · OpenCV · Machine Learning Algorithms
```

**Technologies**

```
Sensor Fusion · AI-Based Activity Recognition
Smart Lighting Automation · Cloud Connectivity · Wireless Communication
```

---

## Requirements

### Hardware

- MYOSA Board
- RGB LED Strips
- APDS9960, MPU6050, BMP180 Sensors
- RTC Module
- Power Supply
- Connecting Wires
- Breadboard / PCB

### Software

- Python 3.x
- Arduino IDE
- ESP32 Drivers
- Flask, NumPy, Pandas, OpenCV

---

## Installation

### 1. Hardware Setup

Connect the following components to the MYOSA board:

```
APDS9960 Sensor    MPU6050 Sensor    BMP180 Sensor
OLED Display       RGB LED Strip     MOSFET Driver Modules
RTC Module         Power Supply
```

### 2. Install Required Libraries

**Arduino Libraries**

```
Adafruit_APDS9960    Adafruit_MPU6050
Adafruit_BMP085      FastLED
WiFi                 BluetoothSerial
```

**Python Libraries**

```bash
pip install flask numpy pandas opencv-python
```

### 3. Upload Firmware

Upload the firmware to the ESP32-based MYOSA board using Arduino IDE.

### 4. Configure Wi-Fi

Update Wi-Fi credentials in the configuration file for remote monitoring and cloud synchronization.

### 5. Start the System

Power on the system and observe adaptive lighting behavior based on user activity and environmental conditions.

---

## Contribution Notes

Contributors can improve CIRCADIA-SENSE by:

- Enhancing activity recognition algorithms
- Improving machine learning models
- Adding advanced health analytics
- Optimizing energy management
- Developing mobile applications
- Integrating additional IoT devices

Contributions, issue reports, and feature suggestions are welcome.

---

## License

This project is developed for educational, research, and innovation purposes under the IEEE MYOSA platform. CIRCADIA-SENSE encourages open-source learning, research collaboration, and academic usage.

---

## Acknowledgements

We sincerely thank the IEEE MYOSA organizing committee for providing a platform that encourages innovation in embedded systems, IoT, healthcare, and sustainable technologies. We also express our gratitude to Chennai Institute of Technology for supporting our research and development efforts. Special thanks to our faculty mentor for guiding us throughout the development of CIRCADIA-SENSE.

---

<div align="center">

CIRCADIA-SENSE &nbsp;·&nbsp; Chennai Institute of Technology &nbsp;·&nbsp; IEEE MYOSA

</div>
