# NTP GPS Stratum 1 Server - ESP32-C3

Servidor NTP de estrato 1 utilizando GPS con PPS, pantalla OLED y ESP32-C3.  
Con funcionamiento automático en modo WiFi cliente (STA) o punto de acceso (AP) cuando no se detecta red disponible.

---

## 📦 Características principales

- Sincronización precisa de tiempo usando GPS + PPS.
- Servidor NTP accesible en red local o a través de AP propio.
- Pantalla OLED 128x64 muestra:
  - Hora y fecha UTC
  - Estado de la señal GPS
  - Estado de conexión WiFi o AP
- Fallback automático si no hay WiFi → crea su propio Access Point.
- Watchdog activo para máxima estabilidad en campo.
- Control de versiones automático (`version.h`).

---

## 🛠️ Requisitos de hardware

- ESP32-C3 DevKitM-1
- Módulo GPS con salida PPS (ejemplo: u-blox NEO-6M)
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

*(Puedes reemplazar esta imagen con tu propia fotografía o diagrama)*

![Esquema de conexión](https://via.placeholder.com/800x400?text=Esquema+Conexion+ESP32-C3+GPS+OLED)

---

## 🚀 Instalación y compilación en Visual Studio Code + PlatformIO

### 1. Instalar Visual Studio Code

Descargar e instalar desde [https://code.visualstudio.com/](https://code.visualstudio.com/)

### 2. Instalar PlatformIO en Visual Studio Code

- Abrir Visual Studio Code.
- Ir a la sección **Extensiones** (Ctrl+Shift+X).
- Buscar **"PlatformIO IDE"** e instalar la extensión oficial.

### 3. Clonar el proyecto

```bash
git clone https://github.com/tu-usuario/NTP_GPS_Stratum1.git
```

O descargar el repositorio como `.zip` y extraerlo.

### 4. Abrir el proyecto en VSCode

- Abrir VSCode.
- Menú **Archivo → Abrir Carpeta**.
- Seleccionar la carpeta del proyecto.

### 5. Compilar y subir el firmware

- Conectar el ESP32-C3 vía USB.
- Desde PlatformIO:
  - Clic en **Build** (martillo) para compilar.
  - Clic en **Upload** (flecha) para cargar.
  - Clic en **Monitor** para ver salida serie.

### 6. Nota importante

- Puede ser necesario instalar drivers USB para CH340/CP210x según tu placa.

---

## 📡 Funcionamiento WiFi

- Si el ESP32-C3 **se conecta** a un WiFi conocido → opera como servidor NTP normal.
- Si **no encuentra WiFi** en 10 segundos → **crea un Access Point**:

  - **SSID**: `Servidor_NTP_GPS`
  - **Password**: `12345678`
  - **IP**: `192.168.4.1`

Puedes conectar cualquier dispositivo a este AP y solicitar hora NTP directamente.

---

## 📄 Licencia

MIT License.  
Libre para usar, modificar y mejorar.

---

## ✨ Versión actual

**Firmware versión:** `v1.0.0`  
(Ver archivo `/include/version.h` para detalles de versiones.)

---