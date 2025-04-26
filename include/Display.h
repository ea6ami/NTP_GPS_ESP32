#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>

// Inicializa la pantalla OLED
void Display_Init();

// Actualiza la información mostrada en la OLED (hora, GPS, WiFi)
void Display_Update();

#endif
