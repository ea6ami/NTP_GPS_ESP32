// Archivo: src/NTPServer.cpp

#include "NTPServer.h"
#include "GPS.h"
#include <Arduino.h>
#include <AsyncUDP.h>
#include "WebServer.h"

extern volatile unsigned long ntpRequestCount;

static AsyncUDP udp;

bool NTPServer_Init() {
    if (udp.listen(NTP_PORT)) {
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
    addNTPRequestHistory(GPS_GetTimeString());
    ntpRequestCount++;

    if (packet.length() < 48) {
        return;
    }

    uint8_t buf[48] = {0};
    const uint8_t* data = packet.data();
    for (int i = 0; i < 8; ++i) {
        buf[24 + i] = data[40 + i];
    }

    // Variables para construir la respuesta
    uint32_t epoch;
    uint32_t frac;
    bool gpsSynced = GPS_IsTimeSync(); // Consultamos si tenemos GPS sincronizado
    GPS_GetTime(epoch, frac);

    uint8_t li = 0;           // Leap Indicator: 0 = sin advertencia
    uint8_t stratum = NTP_STRATUM; // Normalmente estrato 1
    if (!gpsSynced || (epoch == 0 && frac == 0)) {
        li = 3;               // 3 = Clock not synchronized
        stratum = 16;         // Stratum 16 indica no sincronizado
    }

    uint8_t ver = 4;
    uint8_t mode = 4; // Server
    buf[0] = (li << 6) | (ver << 3) | (mode);
    buf[1] = stratum;
    buf[2] = data[2];
    buf[3] = (uint8_t)NTP_PRECISION;

    buf[12] = 'G'; buf[13] = 'P'; buf[14] = 'S'; buf[15] = ' ';

    uint32_t secNTP = epoch + NTP_EPOCH_OFFSET;
    uint32_t fracNTP = (uint32_t)((double)frac * 4294.967296);

    // Si estamos sincronizados, rellenamos los timestamps reales
    if (gpsSynced && (epoch != 0 || frac != 0)) {
        buf[16] = (uint8_t)(secNTP >> 24);
        buf[17] = (uint8_t)(secNTP >> 16);
        buf[18] = (uint8_t)(secNTP >> 8);
        buf[19] = (uint8_t)(secNTP & 0xFF);

        buf[20] = (uint8_t)(fracNTP >> 24);
        buf[21] = (uint8_t)(fracNTP >> 16);
        buf[22] = (uint8_t)(fracNTP >> 8);
        buf[23] = (uint8_t)(fracNTP & 0xFF);

        buf[32] = buf[16]; buf[33] = buf[17]; buf[34] = buf[18]; buf[35] = buf[19];
        buf[36] = buf[20]; buf[37] = buf[21]; buf[38] = buf[22]; buf[39] = buf[23];

        buf[40] = buf[16]; buf[41] = buf[17]; buf[42] = buf[18]; buf[43] = buf[19];
        buf[44] = buf[20]; buf[45] = buf[21]; buf[46] = buf[22]; buf[47] = buf[23];
    }
    // Si no estamos sincronizados, dejamos los timestamps en cero (buf ya inicializado a cero arriba)

    packet.write(buf, sizeof(buf));
    packet.flush();
}
