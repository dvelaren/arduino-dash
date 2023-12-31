# Arduino + Dash

This repository implements a Serial communication between an ESP32 and a Python Dash Application running on a PC. The ESP32 reads the temperature and humidity from a DHT11 sensor and sends the data to the PC. The Dash app plots the data in real time.

## Hardware Required
- ESP32 WROOM 32
- DHT11 Sensor

## Software Required
- Python 3.10
- Dash 2.12.1
- Pyserial 3.5

## Features
- Arduino Over The Air Update (OTA)
- ESP32 Dual Core using FreeRTOS:
    - Core 1: WiFi and OTA tasks
    - Core 2: Real Time tasks (blink in this example)
- Real time plot using Dash and Plotly [[1]](https://dash.plotly.com/live-updates)

## Instructions
1. Create a file called `📒secrets.h` inside  `📂arduino/📂serialDHTOTA` directory. Include the following config parameters and edit accordingly:
    ```c
    #define SECRET_SSID "YOUR_SSID"
    #define SECRET_PASS "YOUR_SSID_PASS"
    #define SECRET_OTA_PASS "DESIRED_OTA_PASS"
    ```
2. Upload the code to the ESP32
3. Edit the COM port in the `📂webapp/app.py` file to match the ESP32 port
4. Install the required Python packages
    ```bash
    pip install -r requirements.txt
    ```
5. Run the Dash app
    ```bash
    python app.py
    ```
![Alt text](docs/dashboard.png)