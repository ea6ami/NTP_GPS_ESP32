// Archivo: src/GPS.cpp

#include "GPS.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <time.h>
#include <esp_timer.h>

// Objeto GPS global
static TinyGPSPlus gps;
static HardwareSerial gpsSerial(1);

// Variables de tiempo compartidas
static volatile bool timeSync = false;
static volatile uint32_t currentSecond = 0;
static volatile unsigned long lastPPSMicros = 0;
static volatile bool newGPSfix = false;
static volatile uint32_t gpsEpoch = 0;

// Timeout detección de pérdida de datos GPS
static unsigned long lastGPSDataMillis = 0;
static const unsigned long GPS_DATA_TIMEOUT_MS = 30000; // 30s

static volatile uint64_t lastPPSUs = 0;

void IRAM_ATTR onGPSPPS() {
    // capturamos timestamp en µs con resolución real
    lastPPSUs = esp_timer_get_time();

    if (!timeSync) {
        if (newGPSfix) {
            // primer pulso tras fix
            currentSecond = gpsEpoch + 1;
            timeSync      = true;
            newGPSfix     = false;
        }
    } else {
        // para cada PPS incrementamos el segundo
        currentSecond++;
    }
}

void GPS_Init() {
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
    pinMode(PIN_GPS_PPS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_GPS_PPS), onGPSPPS, RISING);
}

void GPS_Update() {
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
        lastGPSDataMillis = millis();
    }

    if (gps.time.isUpdated() && gps.date.isUpdated()) {
        if (gps.time.isValid() && gps.date.isValid()) {
            uint16_t year = gps.date.year();
            uint8_t month = gps.date.month();
            uint8_t day = gps.date.day();
            uint8_t hour = gps.time.hour();
            uint8_t minute = gps.time.minute();
            uint8_t second = gps.time.second();

            struct tm timeinfo;
            timeinfo.tm_year = year - 1900;
            timeinfo.tm_mon  = month - 1;
            timeinfo.tm_mday = day;
            timeinfo.tm_hour = hour;
            timeinfo.tm_min  = minute;
            timeinfo.tm_sec  = second;
            timeinfo.tm_isdst = 0;
            time_t unixTime = mktime(&timeinfo);

            gpsEpoch = (uint32_t)unixTime;
            newGPSfix = true;
        }
    }

    if (timeSync && (millis() - lastGPSDataMillis > GPS_DATA_TIMEOUT_MS)) {
        timeSync = false; // Perdida de datos GPS
    }
}

bool GPS_IsTimeSync() {
    return timeSync;
}

int GPS_GetSats() {
    return gps.satellites.isValid() ? gps.satellites.value() : 0;
}

void GPS_GetTime(uint32_t &epochSeconds, uint32_t &microseconds) {
    noInterrupts();
    bool synced       = timeSync;
    uint32_t sec      = currentSecond;
    uint64_t ppsStamp = lastPPSUs;
    interrupts();

    if (!synced) {
        epochSeconds = 0;
        microseconds = 0;
        return;
    }

    // fracción NTP = (µs_restantes / 1e6) * 2^32
    // pero si solo necesitas la microparte, devuelves (ppsStamp % 1e6)
    epochSeconds = sec;
    microseconds = ppsStamp % 1000000UL;
}


String GPS_GetTimeString() {
    if (!GPS_IsTimeSync()) return "--:--:--";

    uint32_t epoch, frac;
    GPS_GetTime(epoch, frac);

    time_t t = (time_t)epoch;
    struct tm* tm_info = gmtime(&t);

    char buffer[9];
    sprintf(buffer, "%02d:%02d:%02d", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    return String(buffer);
}
