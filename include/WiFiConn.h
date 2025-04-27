#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include <Arduino.h>
#include <WiFi.h>

void WiFi_Init();
void WiFi_Update();
bool WiFi_IsConnected();
bool WiFi_IsAPMode();
IPAddress WiFi_GetLocalIP();

#endif // WIFI_CONN_H

