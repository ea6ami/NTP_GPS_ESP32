#include "WiFiConn.h"
#include "ConfigManager.h"   // Para acceder a getters
#include <Arduino.h>
#include <WiFi.h>

// Variables globales
static bool isAPMode = false;

// Función que inicializa la conexión WiFi (cliente o AP)
void WiFi_Init() {
    String ssid = ConfigManager_GetWiFiSSID();
    String password = ConfigManager_GetWiFiPassword();

    if (ssid.length() == 0 || password.length() == 0) {
        Serial.println("Fallo en configuración WiFi, creando AP...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ConfigManager_GetAPSSID().c_str(), ConfigManager_GetAPPassword().c_str(), 1, 0, 4);
        isAPMode = true;
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a red WiFi: " + ssid);
    } else {
        Serial.println("No conectado, creando AP...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ConfigManager_GetAPSSID().c_str(), ConfigManager_GetAPPassword().c_str(), 1, 0, 4);
        isAPMode = true;
    }
}

// Función que revisa en cada bucle si se ha perdido la conexión WiFi
void WiFi_Update() {
    if (!isAPMode && WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado, cambiando a Access Point...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ConfigManager_GetAPSSID().c_str(), ConfigManager_GetAPPassword().c_str(), 1, 0, 4);
        isAPMode = true;
    }
}

// Devuelve true si está conectado como cliente WiFi
bool WiFi_IsConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

// Devuelve true si estamos funcionando como Access Point
bool WiFi_IsAPMode() {
    return isAPMode;
}

// Devuelve la IP local actual (ya sea del cliente WiFi o del AP)
IPAddress WiFi_GetLocalIP() {
    if (isAPMode) {
        return WiFi.softAPIP();
    } else {
        return WiFi.localIP();
    }
}
