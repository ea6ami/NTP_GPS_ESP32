#include <Arduino.h>   

#include <esp_task_wdt.h>
#include <esp_system.h>

#include "config.h"
#include "GPS.h"
#include "WiFiConn.h"
#include "NTPServer.h"
#include "Display.h"


void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando sistema...");

    // Iniciar WiFi
    WiFi_Init();
    // Iniciar GPS (serial + interrupcion PPS)
    GPS_Init();
    // Iniciar Display OLED
    Display_Init();
    // Iniciar servidor NTP
    if (!NTPServer_Init()) {
        Serial.println("Servidor NTP no iniciado. Reiniciando...");
        delay(5000);
        ESP.restart();
    }

    // Configurar Watchdog Timer
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);  // activar WDT con reinicio de CPU
    esp_task_wdt_add(NULL);  // añadir loop task al watchdog

    Serial.println("Setup completo. Esperando sincronizacion GPS...");
}

void loop() {
    // Actualizar datos GPS (leer serial NMEA)
    GPS_Update();

    // Actualizar estado WiFi (reconectar si cayó)
    WiFi_Update();

    // Actualizar display OLED (mostrar info)
    Display_Update();

    // Al mantener el loop ligero, no hay bloqueos largos.
    // Añadimos un pequeño delay/yield para dar tiempo a WiFi y otras tareas del sistema.
    delay(10);

    // Resetear watchdog en cada iteración para prevenir reinicio accidental
    esp_task_wdt_reset();
}
