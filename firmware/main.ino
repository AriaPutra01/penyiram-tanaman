#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

const char* ssid = "Nama_WiFi";
const char* password = "Password_WiFi";
const char* websocket_server = "ws://192.168.1.100:3000/ws"; // Sesuaikan dengan IP backend

const int moisturePin = A0;
const int pumpPin = D1;

using namespace websockets;
WebsocketsClient client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    client.onMessage([](WebsocketsMessage message) {
        Serial.println("Pesan dari Server: " + message.data());
        if (message.data() == "{\"pump\":\"ON\"}") {
            digitalWrite(pumpPin, HIGH);
        } else if (message.data() == "{\"pump\":\"OFF\"}") {
            digitalWrite(pumpPin, LOW);
        }
    });

    client.connect(websocket_server);
}

void loop() {
    if (client.available()) {
        int moisture = analogRead(moisturePin);
        int moisturePercent = map(moisture, 1023, 300, 0, 100); 
        moisturePercent = constrain(moisturePercent, 0, 100); // Pastikan tetap 0-100%

        String jsonData = "{\"moisture\":" + String(moisturePercent) + ", \"pump\":\"" + (digitalRead(pumpPin) ? "ON" : "OFF") + "\"}";
        client.send(jsonData);
        Serial.println("Dikirim: " + jsonData);
    }
    client.poll();
    delay(2000); // Kirim data setiap 2 detik
}
