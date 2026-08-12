#pragma once
#include <Arduino.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define TELEGRAM_BOT_TOKEN "8893949982:AAExW_yzNIR28tXPT13jv2ppW9wssbrQnH0"
#define TELEGRAM_CHAT_ID "6774327335"

struct AuthorizedUser {
    const char* uid;
    const char* name;
};

#define AUTHORIZED_USERS { \
    {"01 02 03 04", "Kulsum - Blue Card"}, \
    {"11 22 33 44", "Green Card"}, \
    {"55 66 77 88", "Yellow Card"}, \
    {"A3 B2 C1 D4", "Kulsum Real Card"}, \
    {"12 34 56 78", "Admin"}, \
}

#define RFID_SS_PIN 5
#define RFID_RST_PIN 22
#define SERVO_PIN 13
#define BUZZER_PIN 12
#define LED_GREEN_PIN 14
#define LED_RED_PIN 27

#define DOOR_OPEN_TIME_MS 5000
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSE_ANGLE 0
#define WIFI_TIMEOUT_MS 10000