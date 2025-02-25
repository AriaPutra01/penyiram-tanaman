#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


const char* ssid = "robi";
const char* password = "obet1234";

AsyncWebServer server(81);
AsyncWebSocket ws("/ws");


const int sensorTanah = A0;
const int pompa = 14;        
const int SDA_PIN = 4;  
const int SCL_PIN = 5;  

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

unsigned long lastSendTime = 0;
const int sendInterval = 2000;
int lastMoisture = -1;
String lastPumpState = "";


void scrollText(const char* text) {
    String message = "                " + String(text) + "                "; 
    for (int i = 0; i < message.length() - 15; i++) {
        lcd.setCursor(0, 1);
        lcd.print(message.substring(i, i + 16)); 
        delay(300);
    }
}

String getSensorReadings() {
    int kelembaban = analogRead(sensorTanah);
    int moisturePercent = map(kelembaban, 1023, 300, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);
    
    bool pompaStatus = moisturePercent < 50;
    digitalWrite(pompa, pompaStatus ? LOW : HIGH);

    StaticJsonDocument<200> readings;
    readings["moisture"] = moisturePercent;
    readings["pump"] = pompaStatus ? "ON" : "OFF";

    String jsonString;
    serializeJson(readings, jsonString);
    return jsonString;
}



void notifyClients() {
    String sensorReadings = getSensorReadings();
    ws.textAll(sensorReadings);
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        StaticJsonDocument<128> json;
        DeserializationError error = deserializeJson(json, (char*)data);
        
        if (!error) {
            String pumpCmd = json["pump"].as<String>();

            if (pumpCmd == "ON") {
                digitalWrite(pompa, LOW);
                Serial.println("Pompa ON");
            } else if (pumpCmd == "OFF") {
                digitalWrite(pompa, HIGH);
                Serial.println("Pompa OFF");
            }
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("Client %u terhubung dari %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("Client %u terputus\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}


void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}


void setup() {
    Serial.begin(115200);
    
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ADR-Team XIIPPLG");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    
    pinMode(pompa, OUTPUT);
    digitalWrite(pompa, HIGH);


    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());


    initWebSocket();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "WebSocket Server Ready!");
    });

    server.begin();
}


void loop() {
    ws.cleanupClients();

    int kelembaban = analogRead(sensorTanah);
    int moisturePercent = map(kelembaban, 1023, 300, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    bool pompaStatus = moisturePercent < 50;
    digitalWrite(pompa, pompaStatus ? LOW : HIGH);

    lcd.setCursor(0, 0);
    lcd.print("Kelembaban: ");
    lcd.print(moisturePercent);
    lcd.print("%  ");

    if (moisturePercent < 50) { 
        scrollText("TANAMAN LAPAR Penyiraman ON");
    } else {
        scrollText("TANAMAN KENYANG Penyiraman OFF");
    }
    if (millis() - lastSendTime >= sendInterval) {
        lastSendTime = millis();  
        notifyClients();
    }
}