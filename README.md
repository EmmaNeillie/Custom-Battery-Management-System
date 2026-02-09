# UGR26_Custom_AMS

## Project Overview

This project implements a modular Accumulator Management System (AMS) firmware for the STM32G473RET  MCU. It manages cell vtolages, temperatures, TS current and state of charge (SoC) while providing fault detection, CAN communication and system monitoring.

The firmware is designed for maintainability, robustness and real-time performance. It is structured into independent modules for peripherals, system monitoring, diagnostics and utilities.

## Features

### Periphals

- **CAN1 & CAN2**: Normal mode, 500kbps, Rx FIFO Interrupts, CAN ID filtering
- **I2C3**: 400kHz, connected to board temperature sensor
- **ADC**: 12-bit resolution, reads currents from Hall effect sensor
- **GPIO**: Status LEDs, SDC status, charger enable control
- **Timers**:
    - LPTIM1 for low power periodic tasks (watchdog refresh etc)
    - TIMx for ADC triggering if needed

### System Monitoring
- **Cell Voltage Monitoring**: Overvoltage, undervoltage & cell imbalance detection
- **Temperature Monitoring**: Thermistor conversion and limits tracking
- **Current Monitoring**: Dual-range ADC with EMA filtering
- **SoC Monitoring**: Coulomb counting with OCV-based drift correction
- **SoH Estimation**: Capacity fade tracking and internal resistance trends
- **Balancing Control**: Passive balancing for cells
- **Isolation Fault Detection**: Measures isolation resistance between battery pack and chassis
- **Power Supply Status Monitoring**: Monitors power rails and determines AMS mode
- **Current Limit Calculation**: Dynamic calculation of safe charge/discharge currents
- **Fault Handling**: Latches fault flags and trigger shutdown or CAN fault messages
- **CAN Telemetry**: Sends pack voltage, current, SoC, SoH and fault status periodically

### Error Handling & Robustness
- Watchdog timer refresh in main loop or RTOS task
- Brown-out reset enabled in option bytes
- Fault handlers (HardFault_Handler)
- CAN Bus Off recovery (automatic or manual)
- Diagnostic logging of errors