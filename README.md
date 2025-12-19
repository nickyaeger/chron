# Chron Smartwatch

A firmware application for the Chron smartwatch, built on the Zephyr RTOS platform and targeting the Nordic nRF54L15 microcontroller.

## Overview

Chron is a feature-rich smartwatch that provides health monitoring, activity tracking, and utility features through an intuitive page-based UI system.

## Features

### Sensors & Hardware
- **Heart Rate Monitoring** - Real-time heart rate sensing
- **Inertial Measurement Unit (IMU)** - Motion tracking and gesture detection
- **Temperature Sensor** - Ambient temperature monitoring
- **GPIO Control** - Efficient hardware interface management

### User Interface Pages
- **Home** - Main dashboard view
- **Activity** - Activity and fitness tracking
- **Timer** - Countdown and stopwatch functionality
- **Climate** - Temperature and environmental data display
- **Summary** - Aggregated health and activity statistics
- **Settings** - Device configuration and preferences

## Technical Stack

- **RTOS:** Zephyr
- **Target Board:** nRF54L15 (Non-Secure CPU App)
- **Build System:** CMake
- **Configuration:** Kconfig

## Project Structure

```
app/
├── src/                 # Application source code
│   ├── main.c           # Main entry point
│   ├── gpio.c/h         # GPIO handling
│   ├── page.c/h         # Page management
│   ├── hr_sensor.c/h    # Heart rate sensor driver
│   ├── imu.c/h          # IMU driver
│   ├── temp_sensor.c/h  # Temperature sensor driver
│   └── pages/           # UI page implementations
├── boards/              # Board-specific configurations
├── CMakeLists.txt       # Build configuration
├── Kconfig              # Kernel configuration
└── prj.conf             # Project configuration
```

## Building

This project uses the Zephyr build system. Ensure you have the nRF Connect SDK and Zephyr toolchain installed.

```bash
west build -b chron_nrf54l15_cpuapp_ns app/
```
