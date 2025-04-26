#include "NTPServer.h"
#include "GPS.h"
#include <Arduino.h>

static AsyncUDP udp;  // Objeto UDP asíncrono (escucha en puerto NTP)

bool NTPServer_Init() {
    if (udp.listen(NTP_PORT)) {
        // Asociar callback de recepción
        udp.onPacket([](AsyncUDPPacket packet) {
            ProcessNTPRequest(packet);
        });
        Serial.println(F("Servidor NTP iniciado en puerto 123 (UDP)"));
        return true;
    } else {
        Serial.println(F("Error: no se pudo abrir puerto UDP 123"));
        return false;
    }
}

void ProcessNTPRequest(AsyncUDPPacket& packet) {
    // Verificar tamaño mínimo del paquete NTP
    if (packet.length() < 48) {
        return; // paquete mal formado, se ignora
    }
    // Buffer para respuesta de 48 bytes
    uint8_t buf[48];
    memset(buf, 0, sizeof(buf));
    // Copiar Originate Timestamp (bytes 24-31) = Transmit Timestamp del cliente (bytes 40-47 de solicitud)
    const uint8_t* data = packet.data();
    for (int i = 0; i < 8; ++i) {
        buf[24 + i] = data[40 + i];
    }

    // LI = 0 (no warning), Version = 4, Mode = 4 (server)
    uint8_t li   = 0;
    uint8_t ver  = 4;
    uint8_t mode = 4;
    buf[0] = (li << 6) | (ver << 3) | (mode);
    buf[1] = NTP_STRATUM;         // Stratum 1 (GPS)
    buf[2] = data[2];             // Poll (copiamos lo que envió el cliente)
    buf[3] = (uint8_t)NTP_PRECISION; // Precision (ya es valor 2-complemento negativo)

    // Root Delay & Root Dispersion (4 bytes cada uno) = 0 en este caso
    // buf[4..7] y buf[8..11] ya están en 0 por memset

    // Reference Identifier (4 bytes) – usamos "GPS "
    buf[12] = 'G'; buf[13] = 'P'; buf[14] = 'S'; buf[15] = ' ';

    // Obtener tiempo actual (UTC epoch y fracción) del módulo GPS
    uint32_t epoch;
    uint32_t frac;
    GPS_GetTime(epoch, frac);
    if (epoch == 0 && frac == 0) {
        // Si no hay tiempo sincronizado, no respondemos (o podríamos enviar stratum=0)
        return;
    }
    // Convertir epoch Unix -> epoch NTP (sumar offset de 70 años)
    uint32_t secNTP = epoch + NTP_EPOCH_OFFSET;
    // Reference Timestamp = tiempo actual (último sync, en este caso mismo que transmitiremos)
    // Colocar secNTP (32 bits) en buf[16..19] y frac (32 bits) en buf[20..23]
    buf[16] = (uint8_t)(secNTP >> 24);
    buf[17] = (uint8_t)(secNTP >> 16);
    buf[18] = (uint8_t)(secNTP >> 8);
    buf[19] = (uint8_t)(secNTP & 0xFF);
    buf[20] = (uint8_t)(frac >> 24);
    buf[21] = (uint8_t)(frac >> 16);
    buf[22] = (uint8_t)(frac >> 8);
    buf[23] = (uint8_t)(frac & 0xFF);

    // Receive Timestamp = secNTP & frac (asumimos recepción casi simultánea)
    buf[32] = buf[16]; buf[33] = buf[17]; buf[34] = buf[18]; buf[35] = buf[19];
    buf[36] = buf[20]; buf[37] = buf[21]; buf[38] = buf[22]; buf[39] = buf[23];
    // Transmit Timestamp = secNTP & frac (igual al actual, mismo que Reference aquí)
    buf[40] = buf[16]; buf[41] = buf[17]; buf[42] = buf[18]; buf[43] = buf[19];
    buf[44] = buf[20]; buf[45] = buf[21]; buf[46] = buf[22]; buf[47] = buf[23];

    // Enviar el paquete de respuesta
    packet.write(buf, sizeof(buf));
    packet.flush();
}
