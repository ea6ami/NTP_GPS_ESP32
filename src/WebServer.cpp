// Archivo: src/WebServer.cpp (adaptado para Dashboard avanzado)

#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "GPS.h"
#include "WebServer.h"
#include "NTPServer.h"

// Variables globales
volatile unsigned long ntpRequestCount = 0;
static AsyncWebServer server(80);

static String wifiSSID = "";
static String wifiIP = "";
static int gpsSatellites = 0;
static String gpsQuality = "Sin señal";
static int ntpStratum = 16;
static String utcTimeString = "--:--:--";
static unsigned long bootMillis = 0;

// Buffer circular para historial de peticiones NTP
#define HISTORY_SIZE 10
static String ntpRequestHistory[HISTORY_SIZE];
static int ntpRequestHistoryIndex = 0;

// Función para agregar una entrada al historial
void addNTPRequestHistory(const String& timestamp) {
    ntpRequestHistory[ntpRequestHistoryIndex] = timestamp;
    ntpRequestHistoryIndex = (ntpRequestHistoryIndex + 1) % HISTORY_SIZE;
}

void WebServer_Init() {
    bootMillis = millis();

    if (!LittleFS.begin()) {
        Serial.println("Error montando LittleFS");
        return;
    }

    // Servir la página principal desde LittleFS
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    
    // Servir el estado JSON actualizado
    server.on("/status.json", HTTP_GET, [](AsyncWebServerRequest *request){
        if (WiFi.status() == WL_CONNECTED) {
            wifiSSID = WiFi.SSID();
            wifiIP = WiFi.localIP().toString();
        } else {
            wifiSSID = WiFi.softAPSSID();
            wifiIP = WiFi.softAPIP().toString();
        }

        gpsSatellites = GPS_GetSats();
        utcTimeString = GPS_GetTimeString();

        if (gpsSatellites >= 5) gpsQuality = "Buena";
        else if (gpsSatellites >= 1) gpsQuality = "Débil";
        else gpsQuality = "Sin señal";

        ntpStratum = (gpsSatellites > 0) ? 1 : 16;

        unsigned long uptimeSec = millis() / 1000;
        char uptimeStr[20];
        snprintf(uptimeStr, sizeof(uptimeStr), "%lu:%02lu:%02lu", uptimeSec/3600, (uptimeSec/60)%60, uptimeSec%60);

        String json = "{";
        json += "\"time\":\"" + utcTimeString + "\",";
        json += "\"satellites\":" + String(gpsSatellites) + ",";
        json += "\"quality\":\"" + gpsQuality + "\",";
        json += "\"ssid\":\"" + wifiSSID + "\",";
        json += "\"ip\":\"" + wifiIP + "\",";
        json += "\"stratum\":" + String(ntpStratum) + ",";
        json += "\"uptime\":\"" + String(uptimeStr) + "\",";
        json += "\"requests\":" + String(ntpRequestCount) + ",";

        // Agregar historial de peticiones NTP
        json += "\"history\":[";
        for (int i = 0; i < HISTORY_SIZE; i++) {
            int idx = (ntpRequestHistoryIndex + i) % HISTORY_SIZE;
            if (ntpRequestHistory[idx].length() > 0) {
                json += "\"" + ntpRequestHistory[idx] + "\"";
                if (i != HISTORY_SIZE - 1) json += ",";
            }
        }
        json += "]";

        json += "}";

        request->send(200, "application/json", json);
    });

    server.begin();
}

void WebServer_Loop() {
    // AsyncWebServer no necesita handleClient()
}