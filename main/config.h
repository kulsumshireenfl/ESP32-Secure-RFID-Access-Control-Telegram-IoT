#pragma once

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

#define TELEGRAM_TOKEN "8893949982:AAExW_yzNIR28tXPT13jv2ppW9wssbrQnH0"
#define TELEGRAM_CHAT_ID "6774327335"

// Your diagram.json pins
#define RC522_SCK 12
#define RC522_MISO 13
#define RC522_MOSI 11
#define RC522_CS 10
#define RC522_RST 4

#define SERVO_PIN 6
#define LED_GREEN 7
#define LED_RED 14
#define BUZZER_PIN 5

typedef struct { const char* uid; const char* name; } user_t;

// Real + Wokwi Fix UIDs
static const user_t authorized_users[] = {
    {"01 02 03 04", "Kulsum - Blue Card actual"},
    {"01 03 04 00", "Kulsum - Blue Card"},
    {"11 22 33 44", "Green Card actual"},
    {"11 33 44 00", "Green Card"},
};
#define AUTHORIZED_USERS_COUNT (sizeof(authorized_users)/sizeof(authorized_users[0]))