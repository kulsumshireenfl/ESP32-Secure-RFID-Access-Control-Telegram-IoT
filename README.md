# ESP32 Secure RFID Door Lock + Telegram IoT - PlatformIO | Wokwi

**Author: Kulsum Shireen | kulsumshireenfl@gmail.com**

Professional IoT Smart Door Lock with RFID + Telegram remote control. Built on ESP32, MFRC522 RFID, Servo Motor using PlatformIO + Arduino framework. 100% Wokwi simulated.

### ✨ Features
- **RFID Access:** MFRC522 RC522 13.56MHz - UID Authentication with Authorized Users List
- **Telegram Control:** Remote Lock/Unlock via Bot - Commands `/lock`, `/unlock`, `/status`
- **Servo Lock:** 0° Locked, 90° Unlocked with auto re-lock after 5 sec
- **Security Alert:** Invalid card -> Buzzer + RED LED + Telegram Alert `⚠️ Invalid RFID attempt`
- **Live Notification:** `🔓 Door Unlocked by Shireen` with UID logged
- **WiFi:** Wokwi-GUEST auto-connect, Secure Telegram API (HTTPS) via WiFiClientSecure
- **Config Managed:** `include/config.h` - All pins, tokens, users in one place

### 🔌 Wiring (diagram.json)
- **MFRC522:** VCC->3V3, GND->GND, RST->GPIO4, SDA(SS)->GPIO5, MOSI->GPIO23, MISO->GPIO19, SCK->GPIO18
- **Servo SG90:** VCC->5V, GND->GND, PWM->GPIO13
- **Buzzer:** +->GPIO2 ->GND
- **LED GREEN:** +220Ω->GPIO12->GND (Access Granted)
- **LED RED:** +220Ω->GPIO14->GND (Access Denied)

### 🚀 How to Test (No Hardware)
1. **Build:** `pio run` - Generates firmware.bin
2. **Simulate:** Press `Ctrl+Alt+W` in VS Code -> Wokwi Simulator starts
3. **RFID Test:** Click RFID card in Wokwi -> Default UID `A3 B2 C1 D4` -> Door unlocks, GREEN LED
4. **Invalid Test:** Change UID in Wokwi RFID inspector -> RED LED + Buzzer + Telegram alert
5. **Telegram Test:** Create Bot via @BotFather, add TOKEN in `include/config.h`, chat `/unlock` in Telegram
6. **Serial Monitor:** `pio device monitor` -> See `Card UID: ...` and `Welcome Shireen`

### 💻 Tech Stack
- **PlatformIO:** espressif32 platform, esp32dev board, Arduino framework
- **Libraries:** MFRC522 (miguelbalboa), ESP32Servo, UniversalTelegramBot (Brian Lough), ArduinoJson
- **Pro Structure:** `src/main.cpp` + `include/config.h` + `platformio.ini` + `wokwi.toml` + `diagram.json`

