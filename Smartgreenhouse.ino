#include <Arduino.h>

#include "dht11Sensor.h"
#include "sharedState.h"

void setup() {
  Serial.begin(115200);
  initDHT();
}

void loop() {
  readDHT();
  delay(2000);
}