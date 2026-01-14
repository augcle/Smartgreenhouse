#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"

SharedState state;

void setup() {
  Serial.begin(115200);
  dht11Begin();
}

void loop() {
  dht11Read(state);

  if (state.hasDht) {
    Serial.print("Temp: ");
    Serial.print(state.tempC);
    Serial.print("  Hum: ");
    Serial.println(state.humidityPct);
  } else {
    Serial.println("DHT read failed");
  }

  delay(2000); // OK for now
}
