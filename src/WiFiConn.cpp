#include "WiFiConn.h"
#include <Arduino.h>

static bool isAPMode = false;

void WiFi_Init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long startAttemptTime = millis();

    // Intentar conectar como cliente (STA) durante 10 segundos
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a WiFi!");
    } else {
        Serial.println("No se pudo conectar a WiFi. Creando Access Point...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678");  // Nombre y contraseña del AP
        isAPMode = true;
        Serial.println("AP creado: Servidor_NTP_GPS");
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
