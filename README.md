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

# Features (Detailed)

## 1. Circadian Rhythm Optimization

The system automatically adjusts lighting according to natural human biological cycles. During morning and work hours, the system produces cool white light with higher brightness levels to improve focus and productivity. During evening and night hours, the system gradually shifts toward warmer light tones that reduce blue light exposure and support healthy sleep patterns.

The lighting engine dynamically adjusts color temperatures between **2000K and 6500K** according to time, environmental conditions, and user activity.

---

## 2. Multi-Sensor Context Awareness

CIRCADIA-SENSE uses multiple sensors integrated with the MYOSA board to understand the surrounding environment in real time.

### APDS9960 Sensor
- Ambient light sensing  
- Gesture recognition  
- Natural light intensity analysis  

### MPU6050 Sensor
- Motion detection  
- Occupancy analysis  
- Activity pattern recognition  

### BMP180 Sensor
- Temperature monitoring  
- Atmospheric condition monitoring  
- Environment-aware lighting adjustment  

### OLED Display
- Displays real-time system data  
- Shows active lighting mode  
- Displays energy-saving statistics  

### Buzzer
- Provides system notifications  
- Indicates lighting mode transitions  

---

## 3. Adaptive Activity Recognition

The system intelligently identifies different user activities and changes the lighting accordingly.

### Reading Mode
Provides balanced white lighting optimized for reading comfort and reduced eye strain.

### Work Mode
Uses bright cool lighting to improve concentration and productivity.

### Relaxation Mode
Applies warm ambient lighting to create a calm and stress-free environment.

### Sleep Preparation Mode
Gradually reduces brightness and transitions to warmer colors to support melatonin production.

---

## 4. Intelligent Energy Optimization

The project follows a **“Just Enough Light”** strategy where the system continuously adjusts brightness according to occupancy and daylight availability.

### Energy Optimization Features
- Automatic dimming during daytime  
- Occupancy-based lighting activation  
- Smart brightness adjustment  
- Zone-specific lighting control  
- Predictive energy optimization using AI  

The system can reduce electricity consumption by approximately **30–40%** compared to traditional lighting systems.

---

## 5. Wireless Smart Connectivity

The ESP32 capabilities of the MYOSA board enable advanced wireless communication features.

### Features
- Wi-Fi-based remote monitoring  
- Bluetooth mobile connectivity  
- Cloud synchronization  
- Smart home integration  
- Over-the-air firmware updates  

Users can monitor and control the system remotely through a mobile application or dashboard.

---

## 6. Machine Learning-Based Pattern Analysis

The system stores environmental and occupancy data to learn user behavior patterns over time. AI algorithms analyze lighting preferences, occupancy schedules, and activity patterns to provide intelligent predictive automation.

### Benefits
- Personalized lighting experience  
- Improved automation accuracy  
- Reduced energy wastage  
- Better user comfort  

---

## 7. Multi-Zone Adaptive Lighting

The demonstration prototype includes four independent lighting zones.

### Workspace Area
Productivity-oriented lighting for office or study work.

### Reading Corner
Focused illumination for reading activities.

### Relaxation Zone
Warm ambient lighting for comfort and mood enhancement.

### Transition Space
Motion-sensitive lighting for movement detection and navigation.

Each zone independently adjusts brightness, color temperature, and activation timing.
