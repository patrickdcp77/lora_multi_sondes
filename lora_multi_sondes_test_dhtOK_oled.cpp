/*
 * Ce programme utilise un module Heltec CubeCell HTCC-AB02 pour lire la température et l'humidité d'un capteur DHT22, ainsi que le niveau de la batterie, et envoyer ces données sous forme de trame hexadécimale.
 * La trame de données est envoyée via la communication série et contient les valeurs de température, d'humidité et de niveau de batterie en hexadécimal.
 * 
 * Démarche :
 * 1. Connexion au capteur DHT22 : Le capteur DHT22 est connecté à la broche GPIO5 (D1) et alimenté par la broche VEXT.
 * 2. Lecture des données du capteur DHT22 : La température et l'humidité sont lues toutes les 10 secondes.
 * 3. Lecture du niveau de la batterie : Le niveau de la batterie est lu en millivolts à partir de la broche analogique ADC1.
 * 4. Création de la trame de données : Les valeurs de température, d'humidité et de niveau de batterie sont converties en hexadécimal et concaténées pour former la trame de données.
 * 5. Envoi de la trame de données : La trame de données est envoyée via la communication série et imprimée pour le débogage.
 * 6. Gestion de l'alimentation : La broche VEXT est utilisée pour activer et désactiver l'alimentation du capteur DHT22 afin de réduire la consommation d'énergie.
 * 
 * Bibliothèques utilisées :
 * - DHT sensor library : Pour lire les mesures du capteur DHT22.
 * - Adafruit Unified Sensor : Bibliothèque de capteurs unifiés d'Adafruit.
 * 
 * Auteur : [Votre Nom]
 * Date : [Date]
 */

#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <U8g2lib.h>

// Définir les broches et le type de capteur DHT
#define DHTPIN GPIO5     // GPIO5 (D1) broche à laquelle le capteur DHT est connecté
#define DHTTYPE DHT22   // Définir le type de capteur DHT
#define BATTERY_PIN ADC1 // Broche ADC1 pour la détection de la puissance de la batterie

// Créer une instance du capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Initialiser l'écran OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Déclaration des fonctions
void setup();
void loop();
void readDHT22(float &temperature, float &humidity);
String createDataFrame(float temperature, float humidity, float batteryVoltage);
void sendDataFrame(const String &dataFrame);
String toHexString(float value, int decimalPlaces);
float readBatteryVoltage();
void displayTemperature(float temperature);

void setup() {
  // Initialiser la communication série
  Serial.begin(9600);

  // Configurer la broche VEXT pour alimenter le capteur
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH); // Désactiver l'alimentation des capteurs branchés sur VEXT

  // Initialiser le capteur DHT
  dht.begin();

  // Initialiser l'écran OLED
  u8g2.begin();
}

void loop() {
  float temperature, humidity;

  // Activer l'alimentation des capteurs branchés sur VEXT
  digitalWrite(Vext, LOW);
  delay(3000); // Attendre que le capteur soit prêt

  // Lire les données du capteur DHT22
  readDHT22(temperature, humidity);

  // Lire la tension de la batterie
  float batteryVoltage = readBatteryVoltage();

  // Créer la trame de données
  String dataFrame = createDataFrame(temperature, humidity, batteryVoltage);

  // Envoyer la trame de données
  sendDataFrame(dataFrame);

  // Désactiver l'alimentation des capteurs branchés sur VEXT
  digitalWrite(Vext, HIGH);

  // Afficher la température sur l'écran OLED
  displayTemperature(temperature);

  // Attendre 10 secondes avant de prendre une nouvelle mesure
  delay(10000);
}

// Fonction pour lire les données du capteur DHT22
void readDHT22(float &temperature, float &humidity) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Vérifier si la lecture a échoué
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Erreur de lecture du capteur DHT22 !");
    temperature = 0.0;
    humidity = 0.0;
  } else {
    // Afficher les valeurs lues
    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidité: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}

// Fonction pour lire la tension de la batterie en volts
float readBatteryVoltage() {
  int adcValue = analogRead(BATTERY_PIN); // Lecture brute ADC
  float voltage = (adcValue * 3.3) / 4095; // Conversion en tension (V)
  float batteryVoltage = voltage * 2; // Adapter selon le facteur de division
  Serial.print("Tension de la batterie: ");
  Serial.print(batteryVoltage);
  Serial.println(" V");
  return batteryVoltage;
}

// Fonction pour créer la trame de données
String createDataFrame(float temperature, float humidity, float batteryVoltage) {
  // Convertir la température en une valeur avec 1 décimale et la coder en hexadécimal
  String tempHex = toHexString(temperature, 1);

  // Convertir l'humidité en une valeur entière et la coder en hexadécimal
  String humHex = toHexString(humidity, 0);

  // Convertir la tension de la batterie en hexadécimal
  String batteryHex = toHexString(batteryVoltage, 2);

  // Créer la trame de données
  String dataFrame = batteryHex + tempHex + humHex;
  return dataFrame;
}

// Fonction pour envoyer la trame de données
void sendDataFrame(const String &dataFrame) {
  // Imprimer la trame de données
  Serial.println("Trame de données : " + dataFrame);

  // Exemple d'envoi de la trame via la communication série
  Serial.println("Envoi de la trame de données : " + dataFrame);
  // Vous pouvez remplacer cette partie par l'envoi via LoRa ou un autre protocole
}

// Fonction pour convertir une valeur en hexadécimal
String toHexString(float value, int decimalPlaces) {
  String hexString = "";
  if (decimalPlaces == 0) {
    int intValue = static_cast<int>(value);
    char hexChar[5];
    sprintf(hexChar, "%04X", intValue);
    hexString = String(hexChar);
  } else {
    int intValue = static_cast<int>(value * pow(10, decimalPlaces)); // Convertir en entier avec les décimales spécifiées
    char hexChar[5];
    sprintf(hexChar, "%04X", intValue);
    hexString = String(hexChar);
  }
  return hexString;
}

// Fonction pour afficher la température sur l'écran OLED
void displayTemperature(float temperature) {
  u8g2.clearBuffer();          // Effacer le buffer
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choisir la police
  u8g2.drawStr(0, 10, "Température:"); // Afficher le texte
  u8g2.setCursor(0, 30);
  u8g2.print(temperature);
  u8g2.print(" °C");
  u8g2.sendBuffer();           // Envoyer le buffer à l'écran
}