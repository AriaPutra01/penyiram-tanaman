#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "robi";
const char* password = "obet1234";
const char* websocket_server = "ws://192.168.1.100:3000/ws";

const int sensorTanah = A0;  //  sensor kelembaban tanah
const int pompa = 14;        // D5 (GPIO14) untuk relay

// Pin I2C untuk ESP8266
const int SDA_PIN = 4;  // D2 (GPIO4)
const int SCL_PIN = 5;  // D1 (GPIO5)

using namespace websockets;
WebsocketsClient client;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void scrollText(const char* text) {
    String message = "                "; 
    message += text; 
    message += "                "; 

    for (int i = 0; i < message.length() - 15; i++) {
        lcd.setCursor(0, 1);
        lcd.print(message.substring(i, i + 16)); 
        delay(300);
    }
}

void setup() {
    Serial.begin(115200);

   
    Wire.begin(SDA_PIN, SCL_PIN);

    // Inisialisasi LCD
    lcd.begin(16, 2); 
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ADR-Team XIIPPLG");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.println("Menghubungkan ke WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    // Inisialisasi pin relay
    pinMode(pompa, OUTPUT);
    digitalWrite(pompa, HIGH); 

    // Setup WebSocket
    client.onMessage([](WebsocketsMessage message) {
        Serial.println("Pesan dari Server: " + message.data());
        if (message.data() == "{\"pump\":\"ON\"}") {
            digitalWrite(pompa, LOW); 
            Serial.println("Pompa ON");
        } else if (message.data() == "{\"pump\":\"OFF\"}") {
            digitalWrite(pompa, HIGH); 
            Serial.println("Pompa OFF");
        }
    });

    // Cek koneksi WebSocket
    if (client.connect(websocket_server)) {
        Serial.println("WebSocket Connected");
    } else {
        Serial.println("Gagal terhubung ke WebSocket!");
    }
}

void loop() {
    int kelembaban = analogRead(sensorTanah);
    int moisturePercent = map(kelembaban, 1023, 300, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kelembaban: ");
    lcd.print(moisturePercent);
    lcd.print("%");

    if (kelembaban > 400 && kelembaban < 1023) {
        digitalWrite(pompa, LOW); 
        scrollText("TANAMAN LAPAR Penyiraman ON");
    } else if (kelembaban > 100 && kelembaban <= 400) {
        digitalWrite(pompa, HIGH); 
        scrollText("TANAMAN KENYANG Penyiraman Off");
    }


    if (client.available()) {
        String jsonData = "{\"moisture\":" + String(moisturePercent) + ", \"pump\":\"" + (digitalRead(pompa) ? "OFF" : "ON") + "\"}";
        client.send(jsonData);
        Serial.println("Dikirim: " + jsonData);
    }


    client.poll();
    delay(2000);
}
