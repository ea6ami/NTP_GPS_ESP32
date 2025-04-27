#include "Display.h"
#include "config.h"
#include "GPS.h"
#include "WiFiConn.h"
#include <Wire.h>
#include <time.h>
#include <ConfigManager.h>

// Crear instancia de display OLED (I2C)
static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);

// Función auxiliar para formatear números
static String twoDigits(int val) {
    if(val < 10) return "0" + String(val);
    else return String(val);
}

void Display_Init() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println(F("GPS NTP Server"));
    display.println(F("Inicializando..."));
    display.display();
}

void Display_Update() {
    display.clearDisplay();
    display.setCursor(0,0);

    // Mostrar Hora UTC
    if (GPS_IsTimeSync()) {
        uint32_t epochSec;
        uint32_t frac;
        GPS_GetTime(epochSec, frac);
        time_t t = (time_t)epochSec;
        struct tm *utc = gmtime(&t);
        char buf[20];
        sprintf(buf, "%02d:%02d:%02d UTC", utc->tm_hour, utc->tm_min, utc->tm_sec);
        display.println(buf);
        sprintf(buf, "%04d-%02d-%02d", utc->tm_year+1900, utc->tm_mon+1, utc->tm_mday);
        display.println(buf);
    } else {
        display.println(F("Hora: --:--:--"));
        display.println(F("Fecha: ----/--/--"));
    }

    // Mostrar Estado GPS
    int sats = GPS_GetSats();
    if (GPS_IsTimeSync()) {
        String satStr = "GPS OK, sats:" + String(sats);
        display.println(satStr);
    } else {
        display.println(F("GPS SIN SENAL"));
    }

 // Mostrar Estado WiFi
    if (WiFi_IsConnected()) {
        IPAddress ip = WiFi_GetLocalIP();
        String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
        display.print(F("WiFi OK: "));
        display.println(ipStr);
    } else if (WiFi_IsAPMode()) {
        // Mostrar SSID y Password del AP configurado
        display.print(F("AP: "));
        display.println(ConfigManager_GetAPSSID());
        display.print(F("Clave: "));
        display.println(ConfigManager_GetAPPassword());
        IPAddress apIP = WiFi_GetLocalIP();
        String apIpStr = String(apIP[0]) + "." + String(apIP[1]) + "." + String(apIP[2]) + "." + String(apIP[3]);
        display.print(F("IP: "));
        display.println(apIpStr);
    } else {
        display.println(F("WiFi Reconectando..."));
    }


    display.display();
}
