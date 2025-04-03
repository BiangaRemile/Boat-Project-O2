#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ELDRADO"; // CHANGE IT
const char* password = "amazon123";
bool launch = false;

AsyncWebServer server(80);

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tableau de Bord - Bateau</title>
    <!-- Importation des icônes FontAwesome -->
    <script src="https://kit.fontawesome.com/a076d05399.js" crossorigin="anonymous"></script>
    <style>
        /* Réinitialisation générale */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }

        body {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            background-color: #121212;
            color: #fff;
        }

        h1 {
            margin-bottom: 20px;
            font-size: 2.5rem;
            color: #f1f1f1;
            text-transform: uppercase;
            letter-spacing: 2px;
        }

        /* Conteneur principal */
        .dashboard {
            display: flex;
            flex-direction: column;
            gap: 20px;
            width: 90%;
            max-width: 1000px;
            padding: 20px;
            background: #1e1e1e;
            border-radius: 12px;
            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4);
        }

        /* Section des informations */
        .info-section {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
        }

        .info-card {
            background: #252525;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
            transition: transform 0.2s ease, background 0.3s ease;
        }

        .info-card:hover {
            background: #333333;
            transform: translateY(-5px);
        }

        .info-card i {
            font-size: 2rem;
            color: #00d1ff;
            margin-bottom: 10px;
        }

        .info-card h3 {
            font-size: 1.2rem;
            color: #f1f1f1;
            margin-bottom: 5px;
        }

        .info-card p {
            font-size: 1.5rem;
            font-weight: bold;
            color: #00d1ff;
        }

        /* Section des contrôles */
        .control-section {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            justify-items: center;
        }

        .control-btn {
            background: #00d1ff;
            border: none;
            color: #121212;
            font-size: 1.5rem;
            font-weight: bold;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
            cursor: pointer;
            transition: all 0.2s ease;
            position: relative;
        }

        .control-btn:hover {
            background: #00b8e6;
            transform: scale(1.1);
        }

        .control-btn i {
            font-size: 1.5rem;
        }

        .control-btn span {
            position: absolute;
            bottom: -15px;
            font-size: 0.9rem;
            color: #bbb;
        }

        /* Style spécial pour stop */
        .stop {
            background: #ff4d4d;
        }

        .stop:hover {
            background: #e33e3e;
        }
    </style>
</head>
<body>
    <h1>Tableau de Bord - Bateau</h1>
    <div class="dashboard">
        <!-- Informations sur le bateau -->
        <div class="info-section">
            <div class="info-card">
                <i class="fas fa-tachometer-alt"></i>
                <h3>Vitesse</h3>
                <p id="speed">0 km/h</p>
            </div>
            <div class="info-card">
                <i class="fas fa-compass"></i>
                <h3>Cap</h3>
                <p id="heading">0°</p>
            </div>
            <div class="info-card">
                <i class="fas fa-ruler-combined"></i>
                <h3>Inclinaison</h3>
                <p id="tilt">0°</p>
            </div>
            <div class="info-card">
                <i class="fas fa-battery-full"></i>
                <h3>Autonomie</h3>
                <p id="battery">100%</p>
            </div>
            <div class="info-card">
                <i class="fas fa-exclamation-triangle"></i>
                <h3>Obstacles</h3>
                <p id="obstacles">Aucun</p>
            </div>
        </div>

        <!-- Contrôles du bateau -->
        <div class="control-section">
            <button class="control-btn" onclick="sendCommand('UP')">
                UP
                
            </button>
            <button class="control-btn" onclick="sendCommand('LEFT')">
                LEFT
               
            </button>
            <button class="control-btn stop" onclick="sendCommand('STOP')">
                STOP
              
            </button>
            <button class="control-btn" onclick="sendCommand('RIGHT')">
                RIGHT
               
            </button>
            <button class="control-btn" onclick="sendCommand('DOWN')">
                DOWN
                
            </button>
        </div>
    </div>

    <script>
        // Fonction pour envoyer des commandes au serveur
        function sendCommand(command) {
            console.log(`Commande envoyée: ${command}`);
            fetch(`/command?direction=${command}`)
                .catch(error => console.error("Erreur d'envoi :", error));
        }

        // Gestion des touches du clavier
        document.addEventListener('keydown', function(event) {
            switch(event.key) {
                case 'ArrowUp':
                    sendCommand('UP');
                    break;
                case 'ArrowDown':
                    sendCommand('DOWN');
                    break;
                case 'ArrowLeft':
                    sendCommand('LEFT');
                    break;
                case 'ArrowRight':
                    sendCommand('RIGHT');
                    break;
                case ' ':
                    sendCommand('STOP');
                    break;
            }
        });

        // Simulation des informations du bateau
        function updateBoatStatus() {
            document.getElementById('speed').innerText = Math.floor(Math.random() * 20) + " km/h";
            document.getElementById('heading').innerText = Math.floor(Math.random() * 360) + "°";
            document.getElementById('tilt').innerText = Math.floor(Math.random() * 45) + "°";
            document.getElementById('battery').innerText = Math.floor(Math.random() * 100) + "%";
            document.getElementById('obstacles').innerText = Math.random() > 0.5 ? "Obstacle détecté !" : "Aucun";
        }

        setInterval(updateBoatStatus, 2000);
    </script>
</body>
</html>
)rawliteral";

void setup() {

  Serial.begin(115200);
  WiFi.softAP(ssid, password);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");        // for debugging
    request->send(200, "text/html", htmlPage);
  });

  server.begin();
}

void loop() {
  if (!launch) {
    Serial.println("Point d'accès actif");

    // Affiche l'adresse IP du point d'accès
    Serial.print("Adresse IP du point d'accès : ");
    Serial.println(WiFi.softAPIP());
    launch =! launch;
  }
}