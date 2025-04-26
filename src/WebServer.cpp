// Archivo: src/WebServer.cpp (adaptado para LittleFS)

#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "GPS.h"
#include "WebServer.h"

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

void WebServer_Init() {
    bootMillis = millis();

    if (!LittleFS.begin()) {
        Serial.println("Error montando LittleFS");
        return;
    }

    // Servir index.html desde LittleFS
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Servir JSON dinámico
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
        json += "\"requests\":" + String(ntpRequestCount);
        json += "}";

        request->send(200, "application/json", json);
    });

    server.begin();
}

void WebServer_Loop() {
    // AsyncWebServer no necesita handleClient()
}
