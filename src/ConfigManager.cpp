#include "ConfigManager.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

// Variables internas
static String wifi_ssid = "";
static String wifi_password = "";
static String ap_ssid = "Servidor_NTP_GPS";  // Default
static String ap_password = "12345678";      // Default

bool ConfigManager_Load() {
    if (!LittleFS.begin(true)) {
        Serial.println("Error montando LittleFS");
        return false;
    }

    if (!LittleFS.exists("/config.json")) {
        Serial.println("No existe /config.json");
        return false;
    }

    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile || configFile.isDirectory()) {
        Serial.println("Error abriendo config.json");
        return false;
    }

    size_t size = configFile.size();
    if (size > 2048) {
        Serial.println("Archivo config.json demasiado grande");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
        Serial.print("Error parsing config.json: ");
        Serial.println(error.c_str());
        return false;
    }

    // Extraer datos
    wifi_ssid = doc["wifi_ssid"] | "";
    wifi_password = doc["wifi_password"] | "";
    ap_ssid = doc["ap_ssid"] | "Servidor_NTP_GPS";
    ap_password = doc["ap_password"] | "12345678";

    Serial.println("Configuración cargada correctamente:");
    Serial.println("  WiFi SSID: " + wifi_ssid);
    Serial.println("  AP SSID: " + ap_ssid);

    return true;
}

// Funciones públicas para consultar
String ConfigManager_GetWiFiSSID() {
    return wifi_ssid;
}

String ConfigManager_GetWiFiPassword() {
    return wifi_password;
}

String ConfigManager_GetAPSSID() {
    return ap_ssid;
}

String ConfigManager_GetAPPassword() {
    return ap_password;
}

