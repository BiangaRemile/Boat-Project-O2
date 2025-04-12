#include <Arduino.h>           // Include the Arduino core library
#include <ArduinoJson.h>       // Include ArduinoJson for JSON parsing
#include <WiFi.h>              // Include WiFi library to connect to a network
#include <ESPAsyncWebServer.h> // Include AsyncWebServer for handling HTTP requests
#include "SPIFFS.h"            // Include SPIFFS (file system) for serving files
#include "pins.hpp"            // Include custom pin definitions
#include <utility>             // Include utility for std::pair

// WiFi credentials (replace with your own SSID and password)
const char *ssid = "Mbz";          // WiFi SSID
const char *password = "11111111"; // WiFi password

AsyncWebServer server(80); // Create an instance of AsyncWebServer on port 80
AsyncWebSocket ws("/ws");  // Create an instance of AsyncWebSocket on "/ws"

/**
 * @brief Decodes a JSON string into a component name and state.
 *
 * @param jsonString The JSON string to decode.
 * @return A pair containing the component name and its state.
 */
std::pair<String, int> decodeJson(const String &jsonString)
{
    JsonDocument doc;                                              // Create a JSON document
    DeserializationError error = deserializeJson(doc, jsonString); // Parse the JSON string

    if (error) // If there's an error in parsing
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str()); // Print the error message
        return {"", -1};               // Return an empty string and -1 as error indicators
    }

    const char *component = doc["component"]; // Extract the "component" field
    int state = doc["state"];                 // Extract the "state" field

    return {component, state}; // Return the extracted values as a pair
}

/**
 * @brief Sends data over WebSocket to all connected clients.
 *
 * @param component The name of the component to send.
 * @param state The state of the component to send.
 */
void sendData(const String &component, int state)
{
    JsonDocument doc;             // Create a JSON document
    doc["component"] = component; // Add the "component" field
    doc["state"] = state;         // Add the "state" field

    String data;
    serializeJson(doc, data); // Serialize the JSON document into a string

    ws.textAll(data); // Send the serialized JSON string to all WebSocket clients
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
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type) // Handle different WebSocket event types
    {
    case WS_EVT_CONNECT: // When a client connects
    {
        Serial.printf("[WebSocket] Client #%u connected from %s\n",
                      client->id(),
                      client->remoteIP().toString().c_str()); // Log the client ID and IP address
        break;
    }

    case WS_EVT_DISCONNECT: // When a client disconnects
    {
        Serial.printf("[WebSocket] Client #%u disconnected\n", client->id()); // Log the client ID
        break;
    }

    case WS_EVT_DATA: // When data is received from a client
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;                // Get the frame info
        if (info->final && info->index == 0 && info->len == len) // Ensure it's a complete frame
        {
            if (info->opcode == WS_TEXT) // If the data is text
            {
                data[len] = 0;                          // Null-terminate the data string
                auto decode = decodeJson((char *)data); // Decode the JSON string
                Serial.printf("[WebSocket] Text from #%u: %s => %d\n",
                              client->id(), decode.first.c_str(), decode.second); // Log the decoded data

                if (decode.first == "led") // If the component is "led"
                {
                    digitalWrite(WIFI_LED, decode.second); // Set the LED pin to the received state
                }
            }
            else // If the data is binary
            {
                Serial.printf("[WebSocket] Binary from #%u: %u bytes\n",
                              client->id(),
                              len); // Log the binary data length
            }
        }
        break;
    }

    case WS_EVT_ERROR: // When an error occurs
    {
        Serial.printf("[WebSocket] Error from client #%u: %s\n",
                      client->id(),
                      (char *)data); // Log the error message
        break;
    }

    case WS_EVT_PONG: // When a PONG response is received
    {
        Serial.printf("[WebSocket] Pong from client #%u\n", client->id()); // Log the PONG response
        break;
    }
    }
}

/**
 * @brief Handles HTTP requests for unknown routes.
 *
 * @param request Pointer to the incoming HTTP request.
 */
void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found"); // Respond with a 404 Not Found error
}

/**
 * @brief Initializes the ESP32 and sets up the web server and WebSocket.
 */
void setup()
{
    pinMode(WIFI_LED, OUTPUT); // Set the WiFi LED pin as output

    Serial.begin(9600); // Initialize serial communication at 9600 baud rate

    WiFi.mode(WIFI_STA);        // Set the WiFi mode to station (client)
    WiFi.begin(ssid, password); // Connect to the specified WiFi network

    byte result = WiFi.waitForConnectResult(); // Wait for the connection result

    if (result != WL_CONNECTED) // If the connection fails
    {
        Serial.println("Socket connection failed!"); // Log the failure
        return;
    }
    else // If the connection succeeds
    {
        Serial.println("Socket connected successfully!"); // Log the success
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP()); // Print the local IP address
    }

    if (!SPIFFS.begin(true)) // Mount the SPIFFS file system
    {
        Serial.println("SPIFFS Mount Failed"); // Log the failure
        return;
    }

    if (!SPIFFS.exists("/web_site/joystick_base.png")) // Check if a specific file exists
    {
        Serial.println("Fichier image manquant !"); // Log if the file is missing
    }
    else
    {
        Serial.println("Fichier image trouvé."); // Log if the file is found
    }

    ws.onEvent(onWsEvent);  // Attach the WebSocket event handler
    server.addHandler(&ws); // Add the WebSocket handler to the server

    // Define routes for serving static files
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
        IPAddress ip = request->client()->remoteIP();
        Serial.printf("[%d.%d.%d.%d] GET %s \n", ip[0], ip[1], ip[2], ip[3], request->url());
        request->send(SPIFFS, "/web_site/index.html", "text/html"); }); // Serve the main HTML page

    server.on("/styles/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/styles/style.css", "text/css"); }); // Serve the CSS file

    server.on("/scripts/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/scripts/script.js", "text/javascript"); }); // Serve the JavaScript file

    server.on("/joystick_base.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/joystick_base.png", "image/png"); }); // Serve the base joystick image

    server.on("/joystick_blue.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/joystick_blue.png", "image/png"); }); // Serve the blue joystick image

    server.on("/scripts/websocket.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web_site/scripts/websocket.js", "text/javascript"); }); // Serve the WebSocket JavaScript file

    server.onNotFound(notFound); // Handle unknown routes
    server.begin();              // Start the web server

    while (true) // Infinite loop to periodically send data
    {
        static unsigned long lastSendTime = 0; // Track the last send time
        unsigned long currentTime = millis();  // Get the current time in milliseconds
        if (currentTime - lastSendTime >= 500) // If 0.5 seconds have passed
        {
            lastSendTime = currentTime;             // Update the last send time
            sendData("led", digitalRead(WIFI_LED)); // Send the current state of the LED
        }
        ws.cleanupClients(); // Clean up disconnected WebSocket clients
    }
}

/**
 * @brief The main loop function (not used in this program).
 */
void loop()
{
    // This loop is intentionally left empty because the program logic is handled in the `setup()` function.
}