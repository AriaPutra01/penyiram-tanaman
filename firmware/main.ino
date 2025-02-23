#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Nama_WiFi";
const char* password = "Password_WiFi";
const char* websocket_server = "ws://192.168.1.100:3000/ws"; 

const int sensorTanah = 4;
const int pompa = 5; 

using namespace websockets;
WebsocketsClient client;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    lcd.init();
    lcd.backlight();
    lcd.begin(16, 2);

    pinMode(pompa, OUTPUT);
    digitalWrite(pompa, HIGH); // Pompa default mati

    client.onMessage([](WebsocketsMessage message) {
        Serial.println("Pesan dari Server: " + message.data());
        if (message.data() == "{\"pump\":\"ON\"}") {
            digitalWrite(pompa, LOW);
        } else if (message.data() == "{\"pump\":\"OFF\"}") {
            digitalWrite(pompa, HIGH);
        }
    });

    client.connect(websocket_server);
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
        lcd.setCursor(0, 1);
        lcd.print("Pompa ON");
    } else if (kelembaban > 100 && kelembaban <= 400) {
        digitalWrite(pompa, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("Pompa OFF");
    }

    if (client.available()) {
        String jsonData = "{\"moisture\":" + String(moisturePercent) + ", \"pump\":\"" + (digitalRead(pompa) ? "OFF" : "ON") + "\"}";
        client.send(jsonData);
        Serial.println("Dikirim: " + jsonData);
    }

    client.poll();
    delay(2000);
}
