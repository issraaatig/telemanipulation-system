# Telemanipulation System

Firmware for a smart glove (ESP32-A) that teleoperates a tendon-driven robotic hand (ESP32-B) using ESP-NOW. Both nodes use ESP-IDF v5.x and share common packet and configuration utilities.

## Structure
- docs/: Architecture, protocol, calibration, safety notes.
- shared_components/: Code shared by both firmwares (packet layout, config, utils).
- glove_firmware/: ESP32-A application (sensing + ESP-NOW TX).
- robot_firmware/: ESP32-B application (ESP-NOW RX + servo control via PCA9685).

## Build
Use ESP-IDF v5.x. From each firmware folder, run `idf.py set-target esp32 && idf.py build flash monitor`. Configure MACs and pins in common_config before flashing.

## Safety
Emergency stop, wireless timeout, and hardware watchdog are integrated. Clamp angles 0–90°. Verify pin mappings match your hardware before use.
