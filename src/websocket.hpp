#ifndef __WEBSOCKET__
#define __WEBSOCKET__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "pins.hpp"
#include <ArduinoJson.h>
#include <globals.hpp>

class WebSocket
{
public:
    WebSocket() = delete;

    WebSocket(const String &path) noexcept : _socket(path)
    {
        _socket.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                        { this->onWsEvent(server, client, type, arg, data, len); });
    }

    /**
     * @brief Sends data over WebSocket to all connected clients.
     *
     * @param component The name of the component to send.
     * @param state The state of the component to send.
     */
    void sendData(const String &component, const char *state)
    {
        JsonDocument doc;             // Create a JSON document
        doc["component"] = component; // Add the "component" field
        doc["state"] = state;         // Add the "state" field

        String data;
        serializeJson(doc, data); // Serialize the JSON document into a string

        _socket.textAll(data); // Send the serialized JSON string to all WebSocket clients
    }

    AsyncWebSocket _socket; // Made public to allow access from WebServerManager

private:
    /**
     * @brief Decodes a JSON string into a component name and state.
     *
     * @param jsonString The JSON string to decode.
     * @return A pair containing the component name and its state.
     */
    std::pair<String, int> decodeJson(const String &jsonString)
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonString);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return {"", -1};
        }

        const char *component = doc["component"];
        int state = doc["state"];
        return {component, state};
    }

    /**
     * @brief Handles WebSocket events such as connection, disconnection, and data reception.
     *
     * @param server Pointer to the WebSocket server.
     * @param client Pointer to the WebSocket client.
     * @param type The type of WebSocket event.
     * @param arg Additional argument depending on the event type.
     * @param data The data received (if any).
     * @param len Length of the received data.
     */
    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                   AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        switch (type)
        {
        case WS_EVT_CONNECT:
            Serial.printf("[WebSocket] Client #%u connected from %s\n",
                          client->id(),
                          client->remoteIP().toString().c_str());
            break;

        case WS_EVT_DISCONNECT:
            Serial.printf("[WebSocket] Client #%u disconnected\n", client->id());
            break;

        case WS_EVT_DATA:
        {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len)
            {
                if (info->opcode == WS_TEXT)
                {
                    data[len] = 0;
                    auto decoded = decodeJson((char *)data);

                    if (decoded.first == "servo")
                    {
                        if (xSemaphoreTake(servoMutex, portMAX_DELAY) == pdTRUE)
                        {
                            servoAngle += (int) (decoded.second / 2);
                            if (servoAngle > 180)
                            {
                                servoAngle = 180;
                            }
                            else if (servoAngle < 0)
                            {
                                servoAngle = 0;
                            }
                            xSemaphoreGive(servoMutex);
                        }
                    }
                }
            }
            break;
        }

        case WS_EVT_ERROR:
            Serial.printf("[WebSocket] Error from client #%u: %s\n",
                          client->id(), (char *)data);
            break;

        case WS_EVT_PONG:
            Serial.printf("[WebSocket] Pong from client #%u\n", client->id());
            break;
        }
        }
    };

#endif // __WEBSOCKET__
