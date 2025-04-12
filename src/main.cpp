#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "pins.hpp"

const char *ssid = "Airtel-E36D"; // CHANGE IT
const char *password = "04945749";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
    {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
    }

    case WS_EVT_DISCONNECT:
    {
        Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
        break;
    }

    case WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->final && info->index == 0 && info->len == len)
        {
            if (info->opcode == WS_TEXT)
            {
                data[len] = 0;
                char *command = (char *)data;
                Serial.printf("ws[%s][%u] received text: %s\n", server->url(), client->id(), command);
            }
            else if (info->opcode == WS_BINARY)
            {
                Serial.printf("ws[%s][%u] received binary: %u\n", server->url(), client->id(), len);
            }
        }
        break;
    }

    default:
        break;
    }
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup()
{
    pinMode(WIFI_LED, OUTPUT);

    Serial.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    byte result = WiFi.waitForConnectResult();

    if (result != WL_CONNECTED)
    {
        Serial.println("WiFi connection failed!");
        return;
    }
    else
    {
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    if (!SPIFFS.exists("/web_site/joystick_base.png"))
    {
        Serial.println("Fichier image manquant !");
    }
    else
    {
        Serial.println("Fichier image trouvé.");
    }

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/index.html", "text/html"); });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/style.css", "text/css"); });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/script.js", "text/javascript"); });

    server.on("/joystick_base.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/joystick_base.png", "image/png"); });

    server.on("/joystick_blue.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/joystick_blue.png", "image/png"); });

    server.onNotFound(notFound);
    server.begin();

    while (true)
    {
        byte result = WiFi.waitForConnectResult();
        if (result != WL_CONNECTED)
            digitalWrite(WIFI_LED, LOW);
        else
            digitalWrite(WIFI_LED, HIGH);
    }
}

void loop(){}