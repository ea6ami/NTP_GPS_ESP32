// Archivo: src/NTP_GPS_Stratum1.cpp

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <esp_system.h>

#include "config.h"
#include "GPS.h"
#include "WiFiConn.h"
#include "NTPServer.h"
#include "Display.h"
#include "WebServer.h"

void setup() {
    Serial.begin(115200);
    delay(5000);
    Serial.println("Iniciando sistema...");

    WiFi_Init();
    WebServer_Init();
    GPS_Init();
    Display_Init();

    if (!NTPServer_Init()) {
        Serial.println("Servidor NTP no iniciado. Reiniciando...");
        delay(5000);
        ESP.restart();
    }

    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
    esp_task_wdt_add(NULL);

    Serial.println("Setup completo. Esperando sincronizacion GPS...");
}

void loop() {
    GPS_Update();
    WiFi_Update();
    WebServer_Loop();
    Display_Update();

    delay(10);
    esp_task_wdt_reset();
}
