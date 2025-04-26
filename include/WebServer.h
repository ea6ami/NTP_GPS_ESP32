// Archivo: include/WebServer.h

#ifndef WEBSERVER_H
#define WEBSERVER_H

void WebServer_Init();
void WebServer_Loop();
void addNTPRequestHistory(const String& timestamp);

#endif // WEBSERVER_H
