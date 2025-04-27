#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

bool ConfigManager_Load(); // Cargar el JSON al arrancar

String ConfigManager_GetWiFiSSID();
String ConfigManager_GetWiFiPassword();
String ConfigManager_GetAPSSID();
String ConfigManager_GetAPPassword();

#endif
