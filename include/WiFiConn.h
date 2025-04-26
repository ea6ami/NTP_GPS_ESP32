#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include <WiFi.h>
#include "config.h"

// Inicializa WiFi. Si falla, crea un AP.
void WiFi_Init();

// Actualiza WiFi o AP según corresponda.
void WiFi_Update();

// Devuelve true si conectado como STA.
bool WiFi_IsConnected();

// Devuelve true si funcionando como AP.
bool WiFi_IsAPMode();

// IP local (en STA o AP).
IPAddress WiFi_GetLocalIP();

#endif