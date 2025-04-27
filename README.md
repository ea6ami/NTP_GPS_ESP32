# NTP GPS Stratum 1 Server - ESP32-C3

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32C3-blue.svg" alt="Platform ESP32-C3">
  <img src="https://img.shields.io/badge/Framework-Arduino-green.svg" alt="Framework Arduino">
  <img src="https://img.shields.io/badge/License-MIT-lightgrey.svg" alt="License MIT">
  <img src="https://img.shields.io/badge/Status-Active-brightgreen.svg" alt="Project Status">
  <img src="https://img.shields.io/badge/WebDashboard-Yes-blueviolet.svg" alt="Web Dashboard">
</p>

Servidor NTP de estrato 1 utilizando GPS con PPS, pantalla OLED y ESP32-C3.
Con funcionamiento automático en modo WiFi cliente (STA) o punto de acceso (AP) cuando no se detecta red disponible.

---

## 🎟️ Características principales

- Sincronización precisa de tiempo usando GPS + PPS.
- Servidor NTP accesible en red local o a través de AP propio.
- Pantalla OLED 128x64 muestra:
  - Hora y fecha UTC
  - Estado de la señal GPS
  - Estado de conexión WiFi o AP
- Interfaz web integrada para monitorizar:
  - Hora UTC en vivo
  - Satélites visibles
  - Calidad de señal GPS
  - Estado de WiFi y IP
  - Estado del servidor NTP (stratum, uptime, peticiones)
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
git clone https://github.com/ea6ami/NTP_GPS_Stratum1.git
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

## 🛠️ Gestión de configuración WiFi dinámica (LittleFS + config.json)

Este proyecto permite almacenar tanto la **configuración WiFi cliente** como la **configuración del Access Point (AP)** en un archivo JSON externo, ubicado en el sistema de archivos del ESP32 (LittleFS).

Así podrás cambiar WiFi o el AP **sin necesidad de recompilar el firmware**.

### 1. Archivo `config.json`

Dentro de la carpeta `/data/` del proyecto, crear o editar el archivo:

```json
{
  "wifi_ssid": "MiRedWifi",
  "wifi_password": "MiClaveSecreta",
  "ap_ssid": "Servidor_NTP_GPS",
  "ap_password": "12345678"
}
```

- **wifi_ssid** y **wifi_password**: Datos de tu red WiFi a la que el ESP32-C3 intentará conectarse al arrancar.
- **ap_ssid** y **ap_password**: Datos del Access Point (nombre y contraseña) que el ESP32-C3 creará si no puede conectarse al WiFi.

### 2. Subir el Filesystem al ESP32

- En PlatformIO:
  - Clic en **PlatformIO** (icono de hormiga 🐜).
  - Elegir **Upload Filesystem Image**.

Esto cargará el archivo `config.json` en el sistema de archivos LittleFS del ESP32.

🔥 **Nota:** No es necesario instalar librerías adicionales para LittleFS, ya está incluido en el framework oficial de ESP32 (`arduinoespressif32`).

### 3. Funcionamiento en el firmware

- Al arrancar, el ESP32:
  - **Primero intenta conectarse** al WiFi configurado.
  - Si falla, **crea el AP** con los datos configurados en el mismo `config.json`.
- Toda esta configuración es **dinámica**: puedes modificar `config.json` y re-subirlo sin necesidad de reprogramar el firmware.

---

## 📋 Resumen rápido

| Situación | Acción del dispositivo |
|:---|:---|
| Encuentra el WiFi | Se conecta como cliente STA |
| No encuentra el WiFi | Crea su propio Access Point usando `ap_ssid` y `ap_password` |

---

## 💻 Acceso Web de Monitoreo

Una vez el ESP32 esté operativo (ya sea en modo STA o AP), puedes acceder desde cualquier navegador web a su IP local:

- **URL**: `http://<IP-DEL-ESP32>/`

La página mostrará en tiempo real:

- Hora UTC actualizada
- Número de satélites recibidos
- Calidad de señal GPS (Buena, Débil, Sin señal)
- Estado de red WiFi o AP
- Stratum (estrato NTP)
- Uptime del dispositivo
- Histórico de últimas 10 peticiones NTP

🔄 Sin dependencias externas. Funcionamiento rápido y ligero.

---

## 📄 Licencia

MIT License.
Libre para usar, modificar y mejorar.

---

## ✨ Versión actual

**Firmware versión:** `v1.2.0`
(Ver archivo `/include/version.h` para detalles de versiones.)

---

