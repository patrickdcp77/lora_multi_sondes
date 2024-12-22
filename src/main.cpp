#include <Arduino.h>
/*
le système serait dans un boîtier
dht22 pour température et humidité
capteur de luminosité
capteur de mouvement





*/
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}