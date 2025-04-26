# NTP GPS Stratum 1 Server - ESP32-C3

Servidor NTP de estrato 1 utilizando GPS con señal PPS, pantalla OLED y ESP32-C3.  
Con funcionamiento automático en modo WiFi cliente (STA) o punto de acceso (AP) cuando no se detecta red disponible.

---

## 📦 Características principales

- Sincronización precisa de tiempo usando GPS + PPS.
- Servidor NTP accesible en red local o a través de AP propio.
- OLED 128x64 muestra:
  - Hora y fecha UTC
  - Estado de la señal GPS
  - Estado de conexión WiFi o AP
- Fallback automático si no hay WiFi → crea su propio Access Point.
- Watchdog activo para máxima estabilidad en campo.
- Control de versiones automático (`version.h`).

---

## 🛠️ Requisitos de hardware

- ESP32-C3 DevKitM-1
- Módulo GPS con salida PPS (por ejemplo u-blox NEO-6M o mejor)
- Pantalla OLED I2C 128x64 (opcional, recomendado)

---

## ⚡ Conexiones recomendadas

| ESP32-C3 | Módulo GPS | Pantalla OLED |
|:--------|:-----------|:--------------|
| GPIO 20 | TX (GPS)   | SDA (OLED)     |
| GPIO 21 | RX (GPS)   | SCL (OLED)     |
| GPIO 10 | PPS (GPS)  | -              |
| GND     | GND        | GND            |
| 3.3V    | VCC        | VCC            |

---

## 🖼️ Esquema de conexión

