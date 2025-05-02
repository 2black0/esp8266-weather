# 🌤️ Weather Station ESP8266

A smart weather station using **NodeMCU ESP8266**, **DHT11**, and a **rainfall sensor**, capable of:

- Measuring **air temperature**, **humidity**, and **rainfall**
- Sending data to **ThingSpeak** for real-time graph visualization
- Sending **Telegram notifications** when certain thresholds are exceeded
- Providing live sensor readings via **Telegram Bot commands**

---

## 📦 Project Structure

```

weather-station-esp8266/
├── LICENSE
├── README.md
└── projects/
├── code_fix.ino              # Combined main project
├── esp8266_thingspeak.ino    # ThingSpeak upload demo
└── read_sensor.ino           # Sensor reading demo

```

---

## 🔧 Hardware Requirements

| Component           | Description                                 |
|---------------------|---------------------------------------------|
| NodeMCU ESP8266     | Main microcontroller                        |
| DHT11 Sensor        | Temperature & Humidity sensor               |
| Rainfall Sensor     | Bucket tipping rain gauge with interrupt    |
| Wi-Fi Connection    | Required for NTP, ThingSpeak, Telegram API  |

---

## 🔌 Wiring Table

| Component         | NodeMCU Pin | Description                      |
|-------------------|-------------|----------------------------------|
| DHT11 Sensor      | D4 (GPIO2)  | Digital data pin                 |
| Rainfall Sensor   | D1 (GPIO5)  | Connected to interrupt input     |
| Onboard LED       | D4 (GPIO2)  | Active LOW (shared with DHT11)  |

> ⚠️ Use `INPUT_PULLUP` for rainfall sensor interrupt pin to ensure clean signal.

---

## 📡 Features

- 📈 Send temperature, humidity, hourly & daily rainfall to **ThingSpeak** (4 fields)
- 🤖 Integrate with **Telegram Bot**:
  - `/temp` – Show temperature
  - `/hum` – Show humidity
  - `/rain_now` – Today’s rainfall
  - `/rain_hour` – Last hour’s rainfall
  - `/rain_day` – Yesterday’s rainfall
  - `/all` – All of the above
- 🔔 Telegram notifications for:
  - High/Low Temperature
  - High/Low Humidity
  - Heavy Rainfall

---

## 🗂️ File Descriptions

### ✅ `projects/code_fix.ino`
> 🔧 Main integrated sketch

- Reads data from DHT11 and Rainfall sensor
- Uploads 4 fields to ThingSpeak every 2 minutes (via timer interrupt)
- Sends notifications to Telegram when:
  - Temp ≥ 40°C or ≤ 20°C
  - Humidity ≥ 90% or ≤ 70%
  - Hourly rainfall ≥ 30mm
- Supports commands via Telegram

### ✅ `projects/esp8266_thingspeak.ino`
> 📡 Minimal demo: sends 4 random values to ThingSpeak

- Tests channel write using fixed/random numbers
- Can be reused for diagnostics or benchmarking

### ✅ `projects/read_sensor.ino`
> 🌦️ Standalone data reader

- Reads:
  - Temperature & Humidity (DHT11)
  - Rainfall counts & converts to mm
- Tracks:
  - Hourly and daily rainfall
- Syncs time using NTP

---

## 📅 Time Synchronization

Uses `configTime()` with NTP servers:

```cpp
configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
```

Timezone is set to GMT+7. Adjust `timezone` variable as needed.

---

## 🔐 Setup Notes

* Replace Wi-Fi SSID and password:

```cpp
const char* ssid = "YOUR_WIFI";
const char* pass = "YOUR_PASSWORD";
```

* Add your ThingSpeak Channel Number and Write API Key:

```cpp
unsigned long myChannelNumber = 123456;
const char *myWriteAPIKey = "YOUR_API_KEY";
```

* Replace with your own **Telegram Bot Token** and interact with `/start`.

---

## 📈 Example ThingSpeak Fields

| Field | Data                 |
| ----- | -------------------- |
| 1     | Temperature (°C)     |
| 2     | Humidity (%)         |
| 3     | Rainfall Hourly (mm) |
| 4     | Rainfall Daily (mm)  |

---

## 🔔 Example Telegram Message

```text
********* WARNING *********
Rainfall is HIGH !! 32.4 mm
```

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for more details.

---

## 🙋‍♂️ Author

Developed by **Ardy Seto**
Project for smart IoT-based environmental monitoring using ESP8266.
