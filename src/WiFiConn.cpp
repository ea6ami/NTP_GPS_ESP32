#include "WiFiConn.h"
#include <Arduino.h>
#include <LittleFS.h>        // Archivo para acceder al sistema de archivos
#include <ArduinoJson.h>     // Archivo para manejar JSON

static bool isAPMode = false;
static String wifi_ssid = "";
static String wifi_password = "";

bool loadWiFiConfig() {
    if (!LittleFS.begin(true)) {  // OJO: true para formatear si falla
        Serial.println("Error montando LittleFS");
        return false;
    }

    if (!LittleFS.exists("/config.json")) {
        Serial.println("No existe /config.json");
        return false;
    }

    File configFile = LittleFS.open("/config.json", "r", false);
    if (!configFile || configFile.isDirectory()) {
        Serial.println("Error abriendo config.json");
        return false;
    }

    size_t size = configFile.size();
    if (size > 1024) {
        Serial.println("Archivo config.json demasiado grande");
        return false;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, configFile);

    if (error) {
        Serial.print("Error parsing config.json: ");
        Serial.println(error.c_str());
        return false;
    }

    wifi_ssid = doc["wifi_ssid"].as<String>();
    wifi_password = doc["wifi_password"].as<String>();

    configFile.close();

    Serial.println("SSID leido: " + wifi_ssid);
    return true;
}

void WiFi_Init() {
    if (!loadWiFiConfig()) {
        Serial.println("Fallo en configuración WiFi, creando AP...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678");
        isAPMode = true;
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a red WiFi: " + wifi_ssid);
    } else {
        Serial.println("No conectado, creando AP...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678");
        isAPMode = true;
    }
}

void WiFi_Update() {
    if (!isAPMode && WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, cambiando a Access Point...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678");
        isAPMode = true;
    }
}

bool WiFi_IsConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

bool WiFi_IsAPMode() {
    return isAPMode;
}

IPAddress WiFi_GetLocalIP() {
    if (isAPMode) {
        return WiFi.softAPIP();
    } else {
        return WiFi.localIP();
    }
}
