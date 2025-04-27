#include "WiFiConn.h"
#include <Arduino.h>
#include <LittleFS.h>        
#include <ArduinoJson.h>     

// Variables globales
static bool isAPMode = false;           // Variable que indica si estamos funcionando como Access Point (AP)
static String wifi_ssid = "";            // Nombre de la red WiFi a la que conectarse
static String wifi_password = "";        // Contraseña de la red WiFi

// Función que carga la configuración WiFi desde un archivo JSON en LittleFS
bool loadWiFiConfig() {
    if (!LittleFS.begin(true)) {  // Intenta montar LittleFS; si falla, formatea
        Serial.println("Error montando LittleFS");
        return false;
    }

    if (!LittleFS.exists("/config.json")) { // Verifica si existe el archivo de configuración
        Serial.println("No existe /config.json");
        return false;
    }

    File configFile = LittleFS.open("/config.json", "r", false); // Abre el archivo para lectura
    if (!configFile || configFile.isDirectory()) { // Comprueba si se abrió correctamente
        Serial.println("Error abriendo config.json");
        return false;
    }

    size_t size = configFile.size();
    if (size > 1024) { // Si el archivo es muy grande (>1KB), error
        Serial.println("Archivo config.json demasiado grande");
        return false;
    }

    DynamicJsonDocument doc(1024); // Crea documento JSON en RAM
    DeserializationError error = deserializeJson(doc, configFile); // Intenta parsear el JSON

    if (error) { // Si hubo error leyendo JSON
        Serial.print("Error parsing config.json: ");
        Serial.println(error.c_str());
        return false;
    }

    // Extrae los valores de SSID y Password desde el JSON
    wifi_ssid = doc["wifi_ssid"].as<String>();
    wifi_password = doc["wifi_password"].as<String>();

    configFile.close(); // Cierra el archivo

    Serial.println("SSID leido: " + wifi_ssid); // Muestra el SSID cargado
    return true;
}

// Función que inicializa la conexión WiFi (cliente o AP)
void WiFi_Init() {
    if (!loadWiFiConfig()) {
        Serial.println("Fallo en configuración WiFi, creando AP...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678", 1, 0, 4); // CORREGIDO
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
        WiFi.softAP("Servidor_NTP_GPS", "12345678", 1, 0, 4); // CORREGIDO
        isAPMode = true;
    }
}


// Función que revisa en cada bucle si se ha perdido la conexión WiFi
void WiFi_Update() {
    if (!isAPMode && WiFi.status() != WL_CONNECTED) { // Si estamos en modo estación y perdemos conexión
        Serial.println("WiFi desconectado, cambiando a Access Point...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Servidor_NTP_GPS", "12345678");
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
        return WiFi.softAPIP(); // IP del AP
    } else {
        return WiFi.localIP();  // IP asignada como cliente
    }
}
