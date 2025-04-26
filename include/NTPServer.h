#ifndef NTP_SERVER_H
#define NTP_SERVER_H

#include <AsyncUDP.h>
#include "config.h"

// Inicializa el servidor NTP (UDP async en puerto 123)
bool NTPServer_Init();

// Función de callback para procesar cada paquete NTP entrante
void ProcessNTPRequest(AsyncUDPPacket& packet);

#endif
