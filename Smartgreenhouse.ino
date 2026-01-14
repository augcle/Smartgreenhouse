#include <Arduino.h>

#include "sharedState.h"
#include "dht11Sensor.h"
#include "LightSensor.h"
#include "heater.h"
#include "mister.h"

SharedState state;

static const uint32_t PRINT_MS = 1000;

void setup() {
  Serial.begin(115200);
  delay(200);
  dht11Begin();
  lightInit();
  heaterBegin();
  misterInit();
}

void loop() {
  dht11Read(state);

  lightUpdate(state);
  heaterUpdate(state);

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= PRINT_MS) {
    lastPrint = millis();

    Serial.print("hasDht=");
    Serial.print(state.hasDht ? "true" : "false");
    Serial.print("  tempC=");
    Serial.print(state.tempC, 1);
    Serial.print("  hum=");
    Serial.print(state.humidityPct, 1);
    Serial.print("  lighthours=");
    Serial.print(state.lightHoursToday, 4);
    Serial.print("  lampOn=");
    Serial.print(state.lampOn ? "true" : "false");
    Serial.print("  heaterOn=");
    Serial.println(state.heaterOn ? "true" : "false");
  }
}
