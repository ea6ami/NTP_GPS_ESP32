#ifndef GPS_H
#define GPS_H

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "config.h"

// Inicializa el hardware GPS y variables de tiempo
void GPS_Init();

// Debe llamarse frecuentemente en el loop para leer y parsear datos NMEA
void GPS_Update();

// Indica si el GPS ha proporcionado hora válida y el sistema está sincronizado
bool GPS_IsTimeSync();

// Devuelve el número de satélites en vista según el último mensaje GPS
int GPS_GetSats();

// Devuelve el tiempo UTC actual en formato epoch (segundos desde 1970) y fracción de segundo (microsegundos)
// Utiliza variables atómicas protegidas para evitar incoherencias con la ISR
void GPS_GetTime(uint32_t &epochSeconds, uint32_t &microseconds);

#endif
