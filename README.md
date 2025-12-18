# ESP32 Security Core - BETA!!

Offline-first ESP32-based alarm system with **Master (central unit)** and **Slave (zone expanders)** communicating via **ESP-NOW**.

No cloud, no Home Assistant required. A local **Web UI** is exposed by the Master for arming, disarming, and monitoring zones.

---

## 📦 Project Structure

```
/master.ino        -> ESP32 Master (alarm brain + web UI)
/slave.ino         -> ESP32 Slave (zone expander)
/data/
  ├── index.html   -> Web UI
  ├── style.css    -> UI styles
  └── app.js       -> WebSocket logic
```

---

## 🧠 System Overview

* **ESP32 Master**

  * Alarm logic (ARM / DISARM / ENTRY / ALARM)
  * Local siren output
  * Wi-Fi Access Point
  * Web UI (offline)

* **ESP32 Slave(s)**

  * Read physical sensors (doors, PIR, etc.)
  * Send zone state to Master using ESP-NOW
  * No Wi-Fi or internet required

---

## 🌐 Network & Access (DEFAULTS)

### Master Wi-Fi (Access Point)

| Setting    | Value                |
| ---------- | -------------------- |
| SSID       | `ESP32-ALARM`        |
| Password   | `12345678`           |
| IP Address | `192.168.4.1`        |
| Web UI URL | `http://192.168.4.1` |

> You can connect directly with a phone or PC, even without internet.

---

## 🔐 Web UI Security (DEFAULTS)

| Item       | Default  |
| ---------- | -------- |
| PIN Code   | `1234`   |
| PIN Length | 4 digits |

⚠️ **IMPORTANT:** Change the PIN before real use.

---

## 🚨 Alarm Logic (Current)

* States:

  * `DISARMED`
  * `ARMED`
  * `ENTRY DELAY`
  * `ALARM`

* Entry delay (default): **10 seconds**

* Any active zone during `ARMED` triggers entry delay

* If not disarmed within delay → **ALARM**

* Siren is activated locally by the Master

---

## ⚙️ Customization – MASTER (`master.ino`)

### 1️⃣ Change Siren Pin

```cpp
#define SIREN_PIN 26
```

### 2️⃣ Change Number of Zones

```cpp
#define MAX_ZONES 8
```

### 3️⃣ Change Entry Delay (seconds)

```cpp
#define ENTRY_DELAY 10
```

### 4️⃣ Change Web UI PIN

```cpp
#define PIN_CODE "1234"
```

### 5️⃣ Change Wi-Fi AP Name / Password

```cpp
WiFi.softAP("ESP32-ALARM", "12345678");
```

---

## ⚙️ Customization – SLAVE (`slave.ino`)

Each Slave represents **one zone**.

### 1️⃣ Zone & Node Identification

```cpp
#define NODE_ID 1
#define ZONE_ID 1
```

* `NODE_ID`: unique ID for the ESP32 board
* `ZONE_ID`: zone number (1..MAX_ZONES)

### 2️⃣ Sensor Pin

```cpp
#define ZONE_PIN 14
```

Use `INPUT_PULLUP` (default) for NC/NO contacts.

### 3️⃣ Master MAC Address (MANDATORY)

Replace with the **Master ESP32 MAC address**:

```cpp
uint8_t MASTER_MAC[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};
```

To get the Master MAC, upload this snippet temporarily:

```cpp
Serial.println(WiFi.macAddress());
```

---

## 📡 Communication

* Protocol: **ESP-NOW**
* Direction: Slave → Master
* Payload:

  * Zone number
  * Zone state (OPEN / CLOSED)

Works completely **offline**.

---

## 📂 Web UI Files (`/data`)

These files are uploaded to SPIFFS:

* `index.html` → UI layout
* `style.css` → colors & fonts
* `app.js` → WebSocket + commands

You can fully customize the UI without touching firmware logic.

---

## 🔧 Upload Instructions

1. Flash `master.ino` to ESP32 Master
2. Upload `/data` folder using **ESP32 Sketch Data Upload**
3. Flash `slave.ino` to each ESP32 Slave
4. Power Master first, then Slaves
5. Connect to `ESP32-ALARM` Wi-Fi
6. Open `http://192.168.4.1`

---

## ⚠️ Security Notes

* This is a DIY project
* No encryption on ESP-NOW (by default)
* Use only in trusted local environments
* Change all default credentials

---

## 🚀 Future Extensions (Planned)

* Zone types (instant / delayed / 24h)
* Zone bypass
* Event log
* Physical keypad
* RFID / NFC
* MQTT / Home Assistant (monitor only)
* Battery backup & fault detection

---

## 📜 License

MIT – use, modify, learn, improve.

---

**ESP32 Security Core** – Offline. Local. Yours.
