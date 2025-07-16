# Master-Slave Communication Protocol on BIG8051 – CSC Project

This project implements a **serial communication protocol** based on the **Master/Slave model** using the **BIG8051 development board** and the **RS-485 interface**. Developed as part of the "Communication in Control Systems" course, the system coordinates message exchange between multiple nodes (master and slaves) over a shared UART bus.

## 🎯 Objectives

- Design and implement a custom communication protocol with ASCII HEX encoding
- Transmit and receive messages using UART1 (RS-485), in multiprocessor mode
- Simulate a network with **3 nodes**: **Transmitter**, **Receiver**, and **Coordinator**
- Display messages on **LCD** and interact with a **keypad**
- Manage user input and system status via UART and LCD interface

## 🛠 Hardware and Software

- **BIG8051 board** (C8051F040 MCU by Silicon Labs)
- **UART1 configured for RS-485 communication**
- **LCD 2x16**, 4x4 keypad
- **Keil µVision5** with C51 Compiler
- **Serial debug adapter (USB–RS485)**
- Simulation and debugging using Keil IDE

## 🧱 Protocol Overview

### 🔗 Message Format

- ASCII HEX coded messages
- Structure includes:  
  `HW Destination`, `HW Source`, `Message Type`, `Initial Source`, `Final Destination`, `Length`, `Data`, `Checksum`, `[CR, LF]`

### 🔄 Communication Flow

- **Master node** polls each slave cyclically (Type 0 – inquiry)
- Slaves respond with:
  - Type 0 (no data)
  - Type 1 (data message from keypad)
- Master forwards valid messages to final destination node
- **Checksum (LRC)** used for integrity
- Messages validated with address + error-check and sent with 9th bit (multiprocessor mode)

## 🧠 Implementation Highlights

- `TxMesaj()` – Prepares and sends formatted messages (ASCII HEX)
- `RxMesaj()` – Receives and parses incoming messages, verifies checksum
- `bin2ascii()` / `ascii2bin()` – Format conversion helpers
- `UserIO()` – Keyboard and terminal interaction
- `Afisare_mesaj()` – Message display on LCD and serial monitor

## 🔧 Roles per Node

- **Master:** Network controller, message router, buffer manager
- **Slave (Transmitter):** Sends user input via keypad to target node
- **Slave (Receiver):** Displays received messages and status

## 📈 Results

- Full implementation of transmission, reception, and message routing
- LCD shows status and latest message
- Keypad used for dynamic message entry
- Successfully handled message collisions and error states

## 🔄 Possible Improvements

- Add message acknowledgment and retry logic
- Implement message priority or queueing
- Extend to support token-ring arbitration or broadcast

## 👤 Authors

- **Ștefan Atomulesei** – Transmitter node  
- **Roxana-Maria Alexa** – Receiver node  
- **Robert-Teodor Aioanei** – Coordinator logic  
- **Vlad Stoica** – User interface and debugging  

Bachelor's Program – Automation and Applied Informatics  
Faculty of Automation and Computer Science  
Technical University of Iași, 2025

