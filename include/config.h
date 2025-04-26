#ifndef CONFIG_H
#define CONFIG_H

// ===== Configuración de pines de hardware =====
constexpr int PIN_GPS_RX    = 20;    // RX del ESP32-C3 conectado al TX del GPS
constexpr int PIN_GPS_TX    = 21;    // TX del ESP32-C3 conectado al RX del GPS
constexpr int PIN_GPS_PPS   = 10;    // Pin de entrada para la señal PPS del GPS (pulso por segundo)

constexpr int PIN_OLED_SDA  = 8;     // Pin SDA para pantalla OLED I2C
constexpr int PIN_OLED_SCL  = 9;     // Pin SCL para pantalla OLED I2C
constexpr uint8_t OLED_ADDR = 0x3C;  // Dirección I2C de la pantalla OLED
constexpr int OLED_WIDTH    = 128;
constexpr int OLED_HEIGHT   = 64;


// ===== Configuración de servidor NTP =====
constexpr uint16_t NTP_PORT = 123;       // Puerto UDP para NTP (123 por defecto)
constexpr uint32_t NTP_EPOCH_OFFSET = 2208988800UL; // Segundos entre Epoch NTP(1900) y Unix(1970)

// ===== Otros parámetros =====
constexpr uint8_t  NTP_STRATUM  = 1;     // Stratum del servidor (1 = referencia primaria GPS)
constexpr int8_t   NTP_PRECISION = -20;  // Precisión en log2 (e.g., -20 ~ 1 microsegundo)
constexpr uint32_t GPS_BAUD     = 9600;  // Baud rate por defecto del módulo GPS
constexpr unsigned long GPS_TIMEOUT_MS = 5000; // Tiempo máx sin datos GPS para considerarlo perdido

// Watchdog Timer
constexpr int WDT_TIMEOUT_SECONDS = 10;  // Timeout del watchdog en segundos

#endif
