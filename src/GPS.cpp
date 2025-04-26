#include "GPS.h"
#include <Arduino.h>          // Para micros(), delay()
#include <esp_task_wdt.h>     // Para watchdog

// Objeto GPS global (TinyGPS++)
static TinyGPSPlus gps;
static HardwareSerial gpsSerial(1);  // Usamos UART2 en ESP32-C3 (id=2)

// Variables de tiempo compartidas entre ISR PPS y el loop principal
static volatile bool timeSync = false;          // true una vez que la hora GPS inicial ha sido sincronizada
static volatile uint32_t currentSecond = 0;     // Segundos desde epoch Unix en el último pulso PPS
static volatile unsigned long lastPPSMicros = 0; // Timestamp micros() del último PPS recibido
static volatile bool newGPSfix = false;         // Señala que se obtuvo una nueva hora del GPS (NMEA) a la espera del siguiente PPS
static volatile uint32_t gpsEpoch = 0;          // Epoch (s) de la última hora recibida por NMEA

static unsigned long lastGPSDataMillis = 0;
static const unsigned long GPS_DATA_TIMEOUT_MS = 30000; // 30 segundos


// ISR para el pulso PPS del GPS (se llama en cada flanco ascendente de PPS)
void IRAM_ATTR onGPSPPS() {
    unsigned long now = micros();
    if (!timeSync) {
        // Si aún no se ha sincronizado el tiempo, esperamos a tener un fix válido
        if (newGPSfix) {
            // Sincroniza el tiempo inicial en el instante de este PPS
            currentSecond = gpsEpoch + 1;  // La siguiente segundo después del obtenido
            timeSync = true;
            newGPSfix = false;
        }
        // Si no hay fix GPS todavía, no hacemos nada
    } else {
        // Incrementa el contador de segundos normalmente
        currentSecond++;
    }
    lastPPSMicros = now;
}

// Inicialización del módulo GPS
void GPS_Init() {
    // Configurar el puerto serie GPS
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
    // Configurar pin PPS como entrada
    pinMode(PIN_GPS_PPS, INPUT);
    // Adjuntar interrupción al pin PPS (flanco de subida)
    attachInterrupt(digitalPinToInterrupt(PIN_GPS_PPS), onGPSPPS, RISING);
}

// Lectura y parseo de datos GPS; debe llamarse en cada iteración del loop
void GPS_Update() {
    // Leer todos los bytes disponibles del GPS serial
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
        lastGPSDataMillis = millis(); // <- actualizamos timestamp cuando recibimos datos
    }
    // Verificar si se recibió nueva información de tiempo del GPS
    if (gps.time.isUpdated() && gps.date.isUpdated()) {
        if (gps.time.isValid() && gps.date.isValid()) {
            // Construir epoch (segundos desde 1970) a partir de la fecha y hora UTC proporcionada por GPS
            uint16_t year = gps.date.year();
            uint8_t month = gps.date.month();
            uint8_t day = gps.date.day();
            uint8_t hour = gps.time.hour();
            uint8_t minute = gps.time.minute();
            uint8_t second = gps.time.second();
            // Conversión de fecha/hora a epoch Unix (UTC)
            struct tm timeinfo;
            timeinfo.tm_year = year - 1900;
            timeinfo.tm_mon  = month - 1;
            timeinfo.tm_mday = day;
            timeinfo.tm_hour = hour;
            timeinfo.tm_min  = minute;
            timeinfo.tm_sec  = second;
            timeinfo.tm_isdst = 0;
            time_t unixTime = mktime(&timeinfo);
            // Ajustar de zona local a UTC si fuera necesario (en este caso GPS ya está en UTC)
            gpsEpoch = (uint32_t)unixTime;
            // Marcar flag de nuevo fix obtenido
            newGPSfix = true;
        }
    }    
    // Si no se ha sincronizado aún el tiempo global, no marcar como timeSync hasta que llegue PPS (ver ISR)
    // Si ya está sincronizado, podemos actualizar 'gpsEpoch' cada segundo para corrección (no necesaria aquí dado PPS)
    if (timeSync && millis() - lastGPSDataMillis > GPS_DATA_TIMEOUT_MS) {
        timeSync = false; // Perdimos señal válida de GPS
    }
}

// Indica si el tiempo GPS se ha sincronizado al menos una vez (hay hora válida)
bool GPS_IsTimeSync() {
    return timeSync;
}

// Devuelve número de satélites (0 si no disponible)
int GPS_GetSats() {
    return gps.satellites.isValid() ? gps.satellites.value() : 0;
}

// Obtener tiempo actual UTC (epoch y fracción) de forma segura
void GPS_GetTime(uint32_t &epochSeconds, uint32_t &microseconds) {
    // Sección crítica: copiar variables atómicas
    noInterrupts();  // bloquear interrupciones momentáneamente
    bool synced = timeSync;
    uint32_t sec = currentSecond;
    unsigned long lastMicros = lastPPSMicros;
    interrupts();    // reanudar interrupciones

    if (!synced) {
        // Si no está sincronizado aún, devolvemos 0 (no hay tiempo válido)
        epochSeconds = 0;
        microseconds = 0;
        return;
    }
    // Calcular offset desde el último PPS
    unsigned long now = micros();
    unsigned long delta = (now >= lastMicros) ? (now - lastMicros) 
                                             : (ULONG_MAX - lastMicros + 1 + now);
    if (delta > 1000000UL) {
        // Si pasó más de 1 segundo (posible pérdida de PPS), limitar delta a 1s
        delta = 1000000UL;
    }
    // Calcular parte fraccionaria de segundo en unidades de 2^32
    // (fracción NTP: 2^32 representa 1 segundo completo)
    uint64_t frac64 = (uint64_t)delta * 4294967296ULL / 1000000ULL;
    uint32_t frac = (uint32_t)frac64;
    epochSeconds = sec;
    microseconds = frac;
}
