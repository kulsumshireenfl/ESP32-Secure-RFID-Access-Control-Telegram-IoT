# ESP32 Secure RFID Door Lock - ESP-IDF + Telegram IoT

Secure access control system using ESP32 + MFRC522 RFID reader with real-time Telegram notifications. Built with ESP-IDF v5.4.4.

**Branch:** `esp-idf` | **IDF Target:** esp32s3

### Features
- MFRC522 RFID authentication (SPI driver written in C)
- Solenoid lock / Relay control
- Buzzer & LED status feedback
- WiFi connectivity
- Telegram Bot alerts: Access Granted / Denied / Door Open
- Wokwi simulation ready

### Hardware
- ESP32 DevKit, MFRC522 Module + Tags, 5V Relay / Solenoid Lock, Buzzer

### Wiring
SDA->GPIO5, SCK->GPIO18, MOSI->GPIO23, MISO->GPIO19, RST->GPIO22, Relay->GPIO4
All pins can be changed in main/config.h

### Setup
Set in main/config.h:
WIFI_SSID, WIFI_PASS, TELEGRAM_BOT_TOKEN, TELEGRAM_CHAT_ID

Then:
idf.py build
idf.py -p COMPORT flash monitor

### Limitations - Freelance Demo Version
This is a portfolio/demo version for freelance showcasing, not a certified commercial product.

1. UID-only auth, no encryption, can be cloned
2. Cards are hardcoded in main.c, no database/app
3. Logs only via Telegram, no local storage
4. WiFi required, no GSM fallback
5. No OTA / secure boot in this demo
6. Single door only
7. No tamper alarm

For production I can add: Firebase/MQTT, Telegram /addcard commands, SPIFFS logs, DESFire EV2 encryption, OTA.

### Author
kulsumshireenfl - Embedded & IoT Developer - Available for freelance
