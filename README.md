# Automatic Irrigation System
# Overview
The Automatic Irrigation System is designed to efficiently automate the process of watering plants using advanced technology. The system comprises STM32 microcontrollers functioning as sensor nodes and an ESP32 serving as the gateway. This setup allows for real-time monitoring and control of irrigation based on soil moisture levels.

## Features
- STM32 Nodes: Low-power microcontrollers that collect data from soil moisture sensors and control water flow through actuators (valves or pumps).
- ESP32 Gateway: Acts as a bridge between the STM32 nodes and the Node.js server.
- Node.js Server: Utilizes MongoDB for data storage and handles incoming data from the ESP32 gateway.
- MQTT Protocol: Efficient message passing and data exchange between the ESP32 gateway and the server.
- WebSocket Technology: Enables real-time data updates between the server and frontend application, allowing users to monitor and control the system.
- Remote Monitoring: Users can view soil moisture levels and irrigation activities through a web dashboard or mobile app.

## Components
- STM32F103C8T6 Microcontroller: Used as the sensor node to monitor soil conditions.
- ESP32: Serves as the gateway for communication between nodes and the server.
- Node.js: Backend server that processes data and manages communication.
- MongoDB: Database used to store sensor data.
- MQTT: Protocol for lightweight messaging between the gateway and server.
- WebSocket: Technology for real-time communication between the server and frontend.

## System Requirements

- STM32 Microcontroller
- ESP32
- NodeJS
- MongoDB
- ReactJS

## Acknowledgments
Thanks to the open-source community for the libraries and frameworks used in this project.
