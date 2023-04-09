/*
  Titre      : Projet Final - réalisation d'une boussole à partir des données du capteur GY-521 et affichage web
  Auteur     : KOUBELAN RICHARD AKPAGNI
  Date       : 09/04/2023
  Description: Ce code nous permet de faire d'afficher sur une page web les angles inclinaisons et d'orientation
  Version    : 2.1.7
*/

// Insertion des librairies
#include <Arduino.h>
#include <Wire.h>                 // Librairie de communication I2C
#include <MPU6050.h>             // Librairie du capteur GY-521
#include <WiFi.h>               // Librairie pour se connecter à un point d'accès
#include <ESPAsyncWebServer.h> // Librairie activer le serveur web de ESP32
#include <AsyncTCP.h>         // Librairie de dépendance du serveur Web
#include <AsyncElegantOTA.h> // Librairie de serveur ElegantOTA

// Insertion des identifiants du Point d'accès
  const char* ssid = "122dumaresq";
  const char* password = "dumaresq";

// Création d'un objet du capteur MPU-6050 à partir de la classe
  MPU6050 mpu;

// Création d'un objet du serveur web asynchrone à partir de la classe
  AsyncWebServer server(80);

// Variables pour stocker les données d'inclinaison et d'orientation
  int inclinaison_x = 0;
  int inclinaison_y = 0;
  int inclinaison_z = 0;
  int orientation = 0;


void setup() {

// Initialisation de la communication du moniteur sérial
  Serial.begin(9600);

// Initialisation du capteur MPU-6050
  Wire.begin();
  mpu.initialize();

// Connection au Point d'accès
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau Wi-Fi...");
  }
  Serial.println("Connecté au réseau Wi-Fi");

// Méthode de définition de route et de gestion des requêtes HTTP
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request) {
  String jsonResponse = "{";
  jsonResponse += "\"inclinaison-x\":" + String(inclinaison_x) + ",";
  jsonResponse += "\"inclinaison-y\":" + String(inclinaison_y) + ",";
  jsonResponse += "\"inclinaison-z\":" + String(inclinaison_z) + ",";
  jsonResponse += "\"orientation\":" + String(orientation);
  jsonResponse += "}";

// Réponse HTTP envoyé au client
  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
  response->addHeader("Access-Control-Allow-Origin", "*"); // Ajout d'en-tête CORS
  response->addHeader("Access-Control-Allow-Methods", "GET");
  request->send(response);
  });

// Démarrage du serveur EleganOTA
  AsyncElegantOTA.begin(&server);    
// Démarrage du serveur Web
  server.begin();
  Serial.println(WiFi.localIP()); // Affichage de l'addresse ip de l'esp32
 
}

void loop() {

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

// Calcule des angles de tangage et de roulis à partir des données d'accéléromètre
  float accel_magnitude = sqrt(ax * ax + ay * ay + az * az);
  inclinaison_x = (int)(atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI);
  inclinaison_y = (int)(atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI);
  inclinaison_z = (int)(atan2(sqrt(ax * ax + ay * ay), az) * 180 / PI);

// Calcule de l'orientation à partir des données du capteur 
  orientation = (int)(atan2(gy, gz) * 180 / PI);
  
// Ajout d'un délai pour éviter de surcharger le serveur et le capteur
  delay(1000);
}