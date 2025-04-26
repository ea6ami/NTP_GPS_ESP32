Proyecto: Servidor NTP GPS Stratum 1 con ESP32-C3

1. Requisitos:
   - VSCode con PlatformIO
   - ESP32-C3
   - Módulo GPS (PPS)
   - Pantalla OLED opcional

2. Instalación:
   - Clonar proyecto
   - Abrir en VSCode

3. Compilar y Subir:
   - Clic en Build y Upload en PlatformIO

4. Uso:
   - Si conecta a WiFi conocida, opera en STA.
   - Si no conecta tras 10s, crea su propio AP "Servidor_NTP_GPS".

5. Extras:
   - Watchdog, reconexiones automáticas, información en pantalla OLED.
   - Fácil control de versiones en version.h.
