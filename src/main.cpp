#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>
#include "config.h"

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
Servo doorServo;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, secured_client);

AuthorizedUser users[] = AUTHORIZED_USERS;
int userCount = sizeof(users)/sizeof(users[0]);

String getUserNameByUID(String uid) {
    uid.toUpperCase();
    uid.trim();
    for(int i=0; i<userCount; i++) {
        String storedUID = String(users[i].uid);
        storedUID.toUpperCase();
        storedUID.trim();
        if(storedUID == uid) {
            return String(users[i].name);
        }
    }
    return "";
}
void sendTelegram(String message) {
    if(WiFi.status() == WL_CONNECTED) {
        bot.sendMessage(TELEGRAM_CHAT_ID, message, "Markdown");
        Serial.println("[TELEGRAM] " + message);
    }
}

void setup() {
    Serial.begin(115200);
    SPI.begin(18,19,23,5);
    rfid.PCD_Init();
    doorServo.attach(SERVO_PIN);
    doorServo.write(SERVO_CLOSE_ANGLE);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting WiFi");
    int timeout = 0;
    while(WiFi.status()!= WL_CONNECTED && timeout < 20) { delay(500); Serial.print("."); timeout++; }
    Serial.println("\nWiFi Connected!");
    secured_client.setInsecure();

    sendTelegram("Door System Online Ready to scan.");
    Serial.println("Scan RFID Card...");
}

void loop() {
    if(!rfid.PICC_IsNewCardPresent() ||!rfid.PICC_ReadCardSerial()) return;

    String uid = "";
    for(byte i=0; i<rfid.uid.size; i++) {
        if(i>0) uid += " ";
        if(rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    uid.trim();
    Serial.println("Card UID: " + uid);

    String name = getUserNameByUID(uid);

    if(name!= "") {
        Serial.println("Welcome: " + name);
        digitalWrite(LED_GREEN_PIN, HIGH);
        doorServo.write(SERVO_OPEN_ANGLE);
        sendTelegram("✅ " + name + " Entered UID: " + uid);
        delay(DOOR_OPEN_TIME_MS);
        doorServo.write(SERVO_CLOSE_ANGLE);
        digitalWrite(LED_GREEN_PIN, LOW);
    } else {
        Serial.println("Access Denied!");
        digitalWrite(LED_RED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        sendTelegram("🚨 Intruder Alert! Unknown Card: " + uid);
        delay(2000);
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}