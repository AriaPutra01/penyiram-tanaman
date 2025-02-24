#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

using namespace websockets;

const char* ssid = "robi";
const char* password = "obet1234";

const int sensorTanah = A0;  // Sensor kelembaban tanah
const int pompa = 14;        // D5 relay

const int SDA_PIN = 4;  // D2
const int SCL_PIN = 5;  // D1

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void sendSensorData() {
    int kelembaban = analogRead(sensorTanah);
    int moisturePercent = map(kelembaban, 1023, 300, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);
    
    String jsonData = "{\"moisture\": " + String(moisturePercent) + ", \"pump\": \"" + (digitalRead(pompa) ? "OFF" : "ON") + "\"}";
    webSocket.broadcastTXT(jsonData);
}

void scrollText(const char* text) {
    String message = "                " + String(text) + "                "; 
    for (int i = 0; i < message.length() - 15; i++) {
        lcd.setCursor(0, 1);
        lcd.print(message.substring(i, i + 16)); 
        delay(300);
    }
}

void handleWebSocketMessage(uint8_t num, uint8_t* payload, size_t length) {
    String message = String((char*)payload);
    Serial.println("Pesan dari Klien: " + message);
    
    if (message == "PUMP_ON") {
        digitalWrite(pompa, LOW);
        Serial.println("Pompa ON");
    } else if (message == "PUMP_OFF") {
        digitalWrite(pompa, HIGH);
        Serial.println("Pompa OFF");
    }
    sendSensorData();
}

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ADR-Team XIIPPLG");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    
    pinMode(pompa, OUTPUT);
    digitalWrite(pompa, HIGH);
    
    server.on("/", []() {
        server.send(200, "text/html", "<h1>ESP8266 WebSocket Server</h1><p>Gunakan WebSocket pada port 81</p>");
    });
    server.begin();
    
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
        if (type == WStype_TEXT) handleWebSocketMessage(num, payload, length);
    });
}


unsigned long lastSendTime = 0;
const int sendInterval = 2000; 

void loop() {
    server.handleClient();
    webSocket.loop();

    int kelembaban = analogRead(sensorTanah);
    int moisturePercent = map(kelembaban, 1023, 300, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    bool pompaStatus = moisturePercent < 50;
    digitalWrite(pompa, pompaStatus ? LOW : HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kelembaban: ");
    lcd.print(moisturePercent);
    lcd.print("%");

    if (pompaStatus) {
        scrollText("TANAMAN LAPAR Penyiraman ON");
    } else {
        scrollText("TANAMAN KENYANG Penyiraman OFF");
    }

    if (millis() - lastSendTime >= sendInterval) {
        lastSendTime = millis();
        String jsonData = "{\"moisture\": " + String(moisturePercent) + ", \"pump\": \"" + (pompaStatus ? "ON" : "OFF") + "\"}";
        webSocket.broadcastTXT(jsonData);
    }
}




