# Solar and Fuel Cell Powered DAQ System

<img src="https://github.com/UTT-HCC-Research/IoT_DAQ_Summer2025/blob/main/images/flowchart.png" alt="chart1" width="1000" height="500">

This repository is the official user documentation for the UT Tyler - HCC - ARM Summer 2025 Solar Charging Station Project. The station is designed for charging micro-mobility and portable devices using solar power and hydrogen fuel cells, with an integrated IoT-based data acquisition (DAQ) system.

## Overview
This system is a mobile, off-grid solution that uses:
- A 195W monocrystalline solar panel
- 12.8V 20Ah lithium battery bank
- MPPT charge controller
- 2000W/4000W pure sine wave inverter
- Hydrogen fuel cell system for supplemental power
- IoT-enabled DAQ system for real-time monitoring and data logging

## Safety Guidelines
- Always install in a dry area.
- Use insulated tools and wear PPE.
- Avoid working alone.
- Follow manufacturer guidelines strictly.
- Handle batteries and wiring with care.
- Maintain adequate ventilation.

## System Components
- **Solar Panel**: 195W monocrystalline, outputs 18V at 10.83A
- **Batteries**: 12.8V, 20Ah LiFePO₄ with BMS
- **Inverter**: Converts 12V DC to 120V AC, 2000W/4000W
- **Charge Controller**: Renogy MPPT, 30A max current
- **DAQ System**: Includes Arduino Mega, STM32 IoT Board, INA sensors, thermocouples, multiplexer, TFT LCD, etc.
- **Cart Structure**: Two-level mobile frame with solar panel top mount
- **Hydrogen Fuel Cell Components**: Stack, controller, purge valve, tubing

## Assembly Highlights
- **Hinged Solar Panel**: Adjustable with triangular wooden support frame
- **Component Mounting**: Batteries, inverter, DAQ box, and controllers placed strategically for stability and wiring efficiency

## IoT Cloud Integration
- **Board**: STM32 B-L475E-IOT01A2 with Mbed OS
- **Cloud**: Microsoft Azure IoT Hub + Cosmos DB NoSQL
- **Tools**: Azure CLI, PowerShell, Mbed Studio
- **Features**: Real-time telemetry, data routing via custom endpoints and routes

## Operation Instructions
1. Deploy the cart in sunlight; adjust panel tilt (~30° in Houston).
2. Activate kill switch and monitor MPPT screen.
3. Verify voltage and current with multimeter.
4. Use inverter to charge micro-mobility or portable devices.
5. Monitor environmental data via DAQ sensors.
6. Disconnect hydrogen tube after stack shutdown.

## Sample Code & Configuration
Refer to Appendix A in the documentation for I2C-based INA228 and MCP9601 initialization using Arduino IDE or Mbed Studio.

## Contributors
- [Team Member 1]
- [Team Member 2]
- [Team Member 3]
- [Team Member 4]
- [Team Member 5]

## Institution
**The University of Texas at Tyler**  
Department of Mechanical Engineering  
College of Engineering - Houston Engineering Center
