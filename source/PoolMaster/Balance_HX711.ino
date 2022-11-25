/*
 
 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"
#include <ESP8266WiFi.h>
// ---- MQTT client --------------------------------------------------------------
#include <PubSubClient.h>
WiFiClient wifiClient;
PubSubClient client(wifiClient);

#define LOADCELL_DOUT_PIN  D2
#define LOADCELL_SCK_PIN  D1

HX711 scale;

//#if defined(ESP8266)
#define wifi_ssid "xxx"
#define wifi_password "xxx"
#define mqtt_server "192.168.1.91"
#define mqtt_user ""     //s'il a été configuré sur Mosquitto
#define mqtt_password "" //idem

#define calibration_factor_topic "Balance/Facteur"                  
#define scale_topic "Balance/Pourcentage"

float calibration_factor = 3000; //-7050 a fonctionné pour ma configuration d'échelle maximale de 440 lb 

long lastMsg = 0;     //Horodatage du dernier message publié sur MQTT
long lastRecu = 0;


void setup() {

  setup_wifi();           //On se connecte au réseau wifi
  client.setServer(mqtt_server, 1883);    //Configuration de la connexion au serveur MQTT
 // client.setCallback(callback);  //La fonction de callback qui est executée à chaque réception de message   
  
  Serial.print("GO");
}
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion a ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connexion WiFi etablie ");
  Serial.print("=> Addresse IP : ");
  Serial.print(WiFi.localIP());
}

//Reconnexion
void reconnect() {
  //Boucle jusqu'à obtenur une reconnexion
  while (!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, erreur : ");
      Serial.print(client.state());
      Serial.println(" On attend 5 secondes avant de recommencer");
      delay(5000);
    }
  }

  Serial.begin(115200);
  Serial.println("Schéma d'étalonnage HX711 ");
  Serial.println("Retirer tout le poids de la balance ");
  Serial.println("Après le début des lectures, placez le poids connu sur la balance ");
  Serial.println("Appuyez sur + ou a pour augmenter le facteur d'étalonnage ");
  Serial.println("Appuyez sur - ou z pour augmenter le facteur d'étalonnage ");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Facteur zéro : "); //Cela peut être utilisé pour supprimer le besoin de tarer la balance. Utile dans les projets à échelle permanente. 
  Serial.println(zero_factor);
}
//Connexion au réseau WiFi


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
 // getcalibration_factor();
   
 // getscale.get_units();
    
  client.publish(calibration_factor_topic, String(calibration_factor).c_str(), true); // La vitesse du vent en km/h
  client.publish(scale_topic, String(scale.get_units(), 1).c_str(), true);     // La direction du vent
 
   delay(2000);

  scale.set_scale(calibration_factor); //Ajuster à ce facteur d'étalonnage 
  Serial.print("Pourcentage Bidon restant...: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" %"); //Changez-le en kg et réajustez le facteur d'étalonnage si vous suivez les unités SI comme une personne saine d'esprit 
  Serial.print(" facteur_d'étalonnage : ");
  Serial.print(calibration_factor);
  Serial.println();
delay(5000);
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
}
void callback(char* topic, byte* payload, unsigned int length) {

}
