# STM32F7 I2C1 to I2C2 with UART Project

## Overview

This project implements bidirectional communication between I2C1 and I2C2 interfaces with UART functionality on an STM32F7 microcontroller. It serves as a bridge between I2C and UART interfaces, with data integrity verification using CRC-16-CCITT checksums.

## Features

- **I2C Communication**:
  - I2C1 operates as master, I2C2 operates as slave
  - Bidirectional data transfer between I2C interfaces
  - Automatic CRC-16-CCITT checksum calculation and verification

- **UART Communication**:
  - UART2 for data input/output
  - UART3 for debugging and console output
  - Support for standard input/output (printf, scanf)

- **Data Integrity**:
  - CRC-16-CCITT checksum generation and validation
  - Error reporting for failed transmissions
  - Debug output for transmission status

## Hardware Requirements

- STM32F7 series microcontroller
- Hardware with I2C and UART interfaces
- Connections for UART debugging (UART3)

## Data Flow

1. Data received on UART2 is processed and CRC is calculated
2. Data with CRC is transmitted from I2C1 (master) to I2C2 (slave)
3. I2C2 validates the CRC of received data
4. Valid data is forwarded to UART2 output
5. Status and debug information is output on UART3

## Getting Started with STM32CubeIDE

### Importing the Project

1. Open STM32CubeIDE
2. Click on `File > Import...`
3. Select `Existing Projects into Workspace` and click `Next`
4. Browse to the project directory
5. Select the project and click `Finish`

### Building the Project

1. Right-click on the project in the Project Explorer
2. Select `Build Project`
3. Wait for the build process to complete
4. Check the Console view for any build errors or warnings

### Running the Project

1. Connect your STM32F7 board to your computer via ST-LINK
2. Right-click on the project in the Project Explorer
3. Select `Run As > STM32 C/C++ Application`
4. The application will be flashed to the device and start running

### Debugging the Project

1. Connect your STM32F7 board to your computer via ST-LINK
2. Set breakpoints in your code by double-clicking on the left margin of the code editor
3. Right-click on the project in the Project Explorer
4. Select `Debug As > STM32 C/C++ Application`
5. The debug perspective will open automatically
6. Use the debug controls to step through your code:
   - F5: Step Into
   - F6: Step Over
   - F7: Step Return
   - F8: Resume
7. View variables, registers, and memory in the respective views
8. Monitor UART3 output for debugging information

### Serial Terminal Setup

To view debug output and interact with the application:

1. Connect UART3 pins to a USB-to-Serial converter
2. Open a serial terminal program (PuTTY, Tera Term, etc.)
3. Configure the terminal with:
   - Baud rate: 115200
   - Data bits: 8
   - Stop bits: 1
   - Parity: None
   - Flow control: None

## Code Structure

- **CRC Module**: Handles CRC-16-CCITT calculation, validation, and buffer operations
- **Serial Module**: Manages I2C and UART communications, including callbacks and data processing
- **Main Application**: Coordinates data flow between interfaces

## Debugging Tips

- Use UART3 for viewing debug messages
- Check the Error_Handler() calls to identify issues
- Monitor I2C communication using a logic analyzer if available
- Use the Watch window in STM32CubeIDE to monitor critical variables
- Enable SWV ITM Data Console for additional debugging capabilities

![Project Architecture](<Screenshot 2025-04-06 205224.png>)
