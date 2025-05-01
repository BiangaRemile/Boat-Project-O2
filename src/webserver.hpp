#ifndef __WEBSERVER__
#define __WEBSERVER__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "websocket.hpp"
#include "SPIFFS.h" // Include SPIFFS (file system) for serving files

class WebServerManager
{
public:
    WebServerManager() = delete;
    WebServerManager(const byte &port, const String &path) noexcept
        : _server(port), socket(path) { begin(); } // Initialize the server with the specified port and path

    void begin()
    {
        if (!SPIFFS.begin(true)) // Mount the SPIFFS file system
        {
            Serial.println("SPIFFS Mount Failed");
            while (1)
            {
            } // Block execution
        }

        setupRoutes();
        _server.addHandler(&socket._socket); // Correctly add the internal AsyncWebSocket
        _server.begin();
    }

    WebSocket socket;

private:
    AsyncWebServer _server;

    void setupRoutes()
    {
        _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                   {
                       IPAddress ip = request->client()->remoteIP();
                       Serial.printf("[%d.%d.%d.%d] GET %s\n", ip[0], ip[1], ip[2], ip[3], request->url().c_str());
                       request->send(SPIFFS, "/web_site/index.html", "text/html"); });

        _server.on("/styles/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(SPIFFS, "/web_site/styles/style.css", "text/css"); });

        _server.on("/scripts/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(SPIFFS, "/web_site/scripts/script.js", "text/javascript"); });

        _server.on("/joystick_base.png", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(SPIFFS, "/web_site/joystick_base.png", "image/png"); });

        _server.on("/joystick_blue.png", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(SPIFFS, "/web_site/joystick_blue.png", "image/png"); });

        _server.on("/scripts/websocket.js", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(SPIFFS, "/web_site/scripts/websocket.js", "text/javascript"); });

        _server.onNotFound([](AsyncWebServerRequest *request)
                           { request->send(404, "text/plain", "Not Found"); });
    }
};

#endif // __WEBSERVER__
