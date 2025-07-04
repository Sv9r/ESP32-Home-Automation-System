# ESP32-Home-Automation-System
This is a smart home automation system built using an ESP32 microcontroller. It enables control of an AC bulb and LED through gesture detection, capacitive touch, voice commands via Sinric Pro, and real-time cloud logging to Google Sheets for monitoring and analytics. 

---

## Features

- **Gesture Control**: Use hand gestures to toggle devices.
- **Touch Control**: Capacitive touch sensors for manual toggling.
- **Voice/Cloud Control**: Integrates with Alexa/Google Home via SinricPro.
- **Event Logging**: All actions (with timestamp, device, state, and source) are logged to Google Sheets.
- **Time Sync**: Uses NTP for accurate IST timestamps.

---

## Hardware Requirements

| Component           | Quantity | Notes                           |
|---------------------|----------|---------------------------------|
| ESP32 Dev Board     | 1        | Main Controller                                |
| Relay Module        | 2        | One each for bulb and LED       |
| APDS9960 Sensor     | 1        | For gesture detection           |
| Touch Wires/Pads    | 2        | For touch inputs (GPIO 14, 27)  |
| Bulb + Holder       | 1        |                                 |
| LED                 | 1        |                                 |
| Jumper Wires        | As required|                                 |
| Power Supply        | 1        | Suitable for ESP32 and relays   |

---

## Pin Connections

| Function         | ESP32 Pin | Description           |
|------------------|-----------|-----------------------|
| Relay (Bulb)     | 23        | Active LOW            |
| Relay (LED)      | 25        | Active LOW            |
| Touch (Bulb)     | 14        | Capacitive touch      |
| Touch (LED)      | 27        | Capacitive touch      |
| I2C SDA (APDS9960)| 21       | I2C Data              |
| I2C SCL (APDS9960)| 22       | I2C Clock             |

---
## Circuit Diagram

![Circuit Diagram](Schematic-Circuit.jpg)

---
## Software Requirements

- Arduino IDE (with ESP32 board support)
- Libraries:
  - `Wire.h`
  - `Adafruit_APDS9960.h`
  - `WiFi.h`
  - `SinricPro.h`, `SinricProSwitch.h`
  - `ESP_Google_Sheet_Client.h`
  - `time.h`

---

## Cloud Setup

| Service        | Required Info         | Where to Get It             |
|----------------|----------------------|-----------------------------|
| WiFi           | SSID, Password       | Your router                 |
| SinricPro      | APP_KEY, APP_SECRET, Device IDs | SinricPro dashboard      |
| Google Sheets  | Project ID, Client Email, Private Key, Spreadsheet ID | Google Cloud Console & Sheet |

---

## How It Works

| Action Source | Device   | Gesture/Touch | Cloud/Voice | Logging |
|---------------|----------|---------------|-------------|---------|
| Gesture       | LED      | Up/Down       | No          | Yes     |
| Gesture       | Bulb     | Left/Right    | No          | Yes     |
| Touch         | LED      | Touch Pad     | No          | Yes     |
| Touch         | Bulb     | Touch Pad     | No          | Yes     |
| Voice/Cloud   | Both     | No            | Alexa/Google| Yes     |

---

## Google Sheets Logging Format

| Timestamp           | Device | State | Source   |
|---------------------|--------|-------|----------|
| YYYY-MM-DD HH:MM:SS | Bulb   | ON    | Touch    |
| YYYY-MM-DD HH:MM:SS | LED    | OFF   | Voice    |
| ...                 | ...    | ...   | ...      |

---

## Setup Instructions

1. **Hardware**: Connect all components as per the pin table.
2. **Google Cloud**: Create a service account and enable Google Sheets API. Download JSON and extract credentials.
3. **SinricPro**: Register devices and note down APP_KEY, APP_SECRET, and Device IDs.
4. **Arduino IDE**:
    - Install required libraries.
    - Fill in all credentials in the code.
    - Upload to ESP32.
5. **Monitor**: Use Serial Monitor for debugging and check your Google Sheet for event logs.

---



## System Workflow

1. The ESP32 connects to WiFi and syncs time from an NTP server.
2. Gestures (UP/DOWN for LED, LEFT/RIGHT for AC bulb) are read from the APDS-9960 sensor.
3. Touch sensors on GPIO 14 and 27 provide alternate manual control.
4. Sinric Pro handles cloud-based voice and mobile app control.
5. Each device action is logged in a connected Google Sheet.


---







## Troubleshooting

| Issue                  | Possible Cause             | Solution                        |
|------------------------|---------------------------|----------------------------------|
| No WiFi connection     | Wrong SSID/Password       | Double-check credentials         |
| No Google logging      | API not enabled/credentials wrong | Recheck Google Cloud setup |
| Gesture not detected   | Sensor wiring/placement   | Check I2C connections, orientation|
| Voice not working      | SinricPro setup incomplete| Check device IDs, keys           |

---
## Contributors

- Tanushree Waratkar –  [GitHub](https://github.com/Tanushree-Waratkar) | [LinkedIn](https://www.linkedin.com/in/tanushree-waratkar-872a63270/)

- Shivam Rao – [Github](https://github.com/Sv9r) | [LinkedIn](https://www.linkedin.com/in/shivam-rao-a87b55311/)
---
For further customization or questions, please refer to the code comments or reach out to the community.
