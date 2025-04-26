#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include <WiFi.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

void WiFi_Init();
void WiFi_Update();
bool WiFi_IsConnected();
bool WiFi_IsAPMode();
IPAddress WiFi_GetLocalIP();

#endif
