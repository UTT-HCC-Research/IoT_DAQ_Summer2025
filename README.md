# Solar Charging Station for Micro-Mobility and Portable Devices

<img src="https://github.com/UTT-HCC-Research/IoT_DAQ_Summer2025/blob/main/images/flowchart.png" alt="chart1" width="1000" height="500">

## Team Members
- 
- 
- 
- 
- 
- 

## Institution
**The University of Texas at Tyler**  
Department of Mechanical Engineering  
College of Engineering - Houston Engineering Center

---

## Table of Contents
- [Introduction](#introduction)
- [Safety Precautions](#safety-precautions)
- [System Components](#system-components)
- [System Design & Assembly](#system-design--assembly)
- [Electrical Components Assembly](#electrical-components-assembly)
- [Operating Instructions](#operating-instructions)
- [IoT Cloud Setup & Operation (DISCO IoT Only)](#iot-cloud-setup--operation-disco-iot-only)
- [Deployment & Testing](#deployment--testing)
- [Appendix A: Sample Arduino Code](#appendix-a-sample-arduino-code)
- [Appendices B–E](#appendices-be)

---

## Introduction
This system harnesses solar energy and hydrogen fuel cells to charge devices. It includes:
- An Arduino-based DAQ system for solar/fuel cell voltage and current monitoring (local only)
- A DISCO IoT board for environmental data telemetry via Microsoft Azure.

---

## Safety Precautions
- Install in dry areas
- Use insulated tools and PPE
- Avoid working alone
- Handle batteries and wiring with care
- Maintain regular inspections

---

## System Components

### Charging Station Components
- **195W Solar Panel**
- **12.8V 20Ah LiFePO₄ Battery**
- **2000W/4000W Inverter**
- **Renogy MPPT Charge Controller**
- **Custom Mobile Cart**

### Arduino-Based DAQ (Local Monitoring)
- Arduino Mega 2560 R3
- 3x INA228 Current/Voltage Sensors
- MCP9601 Thermocouple Amplifier
- TFT LCD Display
- TCA9548A I2C Multiplexer
- K-Type Thermocouple

### DISCO IoT-Based Cloud Telemetry
- STM32 B-L475E-IOT01A2 Board
- Built-in sensors (temperature, humidity, pressure, etc.)
- Azure IoT Hub + CosmosDB Integration

### Hydrogen Fuel Cell System
- Stack, controller, purge valve, tubes

---

## System Design & Assembly
- Two-tier cart: batteries + fuel cell on bottom, electronics on top
- Hinged solar panel mount for angle adjustment
- Modular wiring and ventilation cutouts

---

## Electrical Components Assembly
- Solar panel → Charge controller
- Charge controller → Battery
- Battery → Inverter
- Arduino → INA228 & MCP9601 (via TCA9548A)
- DISCO board → Azure IoT Hub setup

---

## Operating Instructions

### Fuel Cell Safety
- Pressure: 0.45–0.55 Bar
- Hydrogen purity ≥99.995%
- Ventilation and purge safety required

### Fuel Cell Operation
1. Connect hydrogen
2. Watch controller lights
3. Connect purge valve
4. Power load
5. Disconnect hydrogen

---

## IoT Cloud Setup & Operation (DISCO IoT Only)

### Phase 1: Azure Setup
- Configure Wi-Fi in `mbed_app.json`
- Register IoT Hub and device
- Confirm telemetry with Mbed Studio or PowerShell

### Phase 2: Send Sensor Data
- Replace `main.cpp` with sensor reading code
- Check Mbed Studio output

### Phase 3: Route to CosmosDB
- Create CosmosDB (NoSQL)
- Define `/DeviceId` partition key
- Route telemetry from IoT Hub

### Results from the Sample Dataset pulled from the System
<img src="https://github.com/UTT-HCC-Research/IoT_DAQ_Summer2025/blob/main/images/plots.png" alt="chart2" width="1000" height="500">

---

## Deployment & Testing

### Solar Charging
- Deploy cart outdoors
- Align panel ~30° tilt
- Verify MPPT or multimeter readings

### Arduino Monitoring (Local Only)
- Display voltage, current, and power from INA228 on LCD

### DISCO IoT Cloud Monitoring
- View telemetry on Azure via CosmosDB

### Load Testing
- Power USB/AC device via inverter

---

## Appendix A: Sample Arduino Code
Includes:
- TCA channel selection
- INA228 and MCP9601 sensor reads
- LCD printout of system readings

---

## Appendices B–E
(Reserved for schematics, BOMs, and glossary)
