# Telemanipulation System

## Overview

This project implements a real-time telemanipulation system based on two ESP32 devices communicating via **ESP-NOW**. A smart glove equipped with flex sensors captures finger motion and transmits processed commands to a robotic hand, which reproduces the motion using multiple servos.

The system forms a complete embedded pipeline from **human gesture → signal processing → wireless communication → robotic actuation**, with a focus on clarity, modularity, and safe behavior.

---

## Architecture

The project is divided into three main parts:

### **Glove Firmware**
Handles the sensing and transmission pipeline:
- reads raw flex sensor data
- filters noisy signals
- applies calibration
- converts values into angles
- sends commands using ESP-NOW

### **Robot Firmware**
Handles reception and actuation:
- receives ESP-NOW packets
- validates incoming data
- maps angles to servo outputs
- applies safety rules
- drives servos via PCA9685

### **Shared Components**
Common modules used by both sides:
- configuration definitions
- packet structure
- utility functions

---

## System Workflow

**Sensors → Filtering → Calibration → Angle Conversion → ESP-NOW → Reception → Validation → Mapping → Safety → Servo Control**

This workflow represents the full control loop from glove input to robotic motion.

---

## Repository Structure


telemanipulation-system/
├── docs/
├── shared_components/
├── glove_firmware/
├── robot_firmware/
└── README.md


- **docs/** → technical notes and supporting documentation  
- **shared_components/** → shared logic between both firmware targets  
- **glove_firmware/** → glove-side application  
- **robot_firmware/** → robot-side application  

---

## Main Features

- ESP32-to-ESP32 communication using **ESP-NOW**
- modular ESP-IDF component-based design
- sensor filtering and calibration pipeline
- angle-based control system
- real-time wireless command transmission
- servo control using PCA9685
- safety-oriented actuation logic
- clean separation between sensing, communication, and control

---

## Safety Concept

The system includes safety mechanisms to ensure controlled behavior:

- angle clamping within safe limits
- timeout detection if communication stops
- controlled output behavior during signal loss
- safe handling before applying actuator commands

This ensures predictable behavior during testing and demonstration.

---

## Build Workflow

The project uses **ESP-IDF v5.5.2** targeting **ESP32**.

Each firmware must be built separately.

### **Build glove firmware**

cd glove_firmware
idf.py set-target esp32
idf.py build

### **Build robot firmware**

cd robot_firmware
idf.py set-target esp32
idf.py build


## Configuration

Before flashing to hardware, configuration values must match the actual setup. This includes:

wireless addressing
GPIO pin assignments
calibration parameters
actuator assumptions
safety limits

These values are centralized in the configuration modules for easy adjustment.

## Development Approach

The project follows a modular embedded development style:

separation of responsibilities between devices
reusable shared components
clear data processing stages
structured flow from sensor input to actuator output
safety-aware control design

This makes the code easier to understand, maintain, and extend.

## Purpose

This repository provides a complete firmware foundation for a smart-glove-based robotic telemanipulation system. It is intended for embedded development, system integration, experimentation, and demonstration of real-time gesture-controlled robotic actuation.