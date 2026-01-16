/**
 * @file Smartgreenhouse.ino
 * @brief Main application wiring together sensor/actuator modules.
 *
 * Flow:
 *  - setup(): init modules + set targets
 *  - loop(): read sensors -> controller -> actuate
 */

#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"
#include "Lightsensor.h"
#include "heater.h"
#include "mister.h"
#include "climateControl.h"


SharedState state;

static const uint32_t PRINT_MS = 1000;

void setup() {
  Serial.begin(115200);
  delay(200);

  dht11Begin();
  lightInit();

  heaterBegin();
  misterInit();
  climateControlBegin();

  // Setting temperature targets
  state.targetTempC = 22.0;
  state.targetHumidityPct = 55.0;
}

void loop() {

  dht11Read(state);
  lightUpdate(state);

  climateControlUpdate(state);

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= PRINT_MS) {
    lastPrint = millis();
    Serial.print("tempC="); Serial.print(state.tempC, 1);
    Serial.print(" hum="); Serial.print(state.humidityPct, 1);
    Serial.print(" heaterOn="); Serial.print(state.heaterOn ? "true" : "false");
    Serial.print(" misterOn="); Serial.print(state.misterOn ? "true" : "false");
    Serial.print(" lampOn="); Serial.print(state.lampOn ? "true" : "false");
    Serial.print(" lightHoursToday="); Serial.println(state.lightHoursToday);
  }
}
