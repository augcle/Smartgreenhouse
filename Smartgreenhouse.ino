/**
 * @file Smartgreenhouse.ino
 * @brief Main application that wires together all sensor + actuator modules.
 *
 * Overall flow:
 *  - setup(): initialize hardware/modules + set default targets
 *  - loop():  read sensors -> run controller -> actuate outputs -> (optional) print status
 */

#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"
#include "Lightsensor.h"
#include "heater.h"
#include "mister.h"
#include "climateControl.h"

/**
 * @brief Global shared state instance.
 *
 * All modules read/write through this object so the program has a single source of truth:
 *  - sensors write temp/humidity/light info
 *  - controller writes desired actuator states (heaterOn/misterOn/lampOn)
 *  - actuators use these states to drive hardware
 */

SharedState state;

/**
 * @brief Print interval for Serial status output (milliseconds).
 *
 * Used to avoid spamming Serial every loop iteration (loop runs very fast).
 */

static const uint32_t PRINT_MS = 1000;

/// @brief Arduino setup function (runs once at boot).

void setup() {
  Serial.begin(115200);             ///<
  delay(200);                       ///<

  dht11Begin();                     ///<
  lightInit();                      ///<

  heaterBegin();                    ///<
  misterInit();                     ///<
  climateControlBegin();            ///<

  
  state.targetTempC = 22.0;         ///< Desired temperature in °C 
  state.targetHumidityPct = 55.0;   ///< Desired humidity in %
}

/**
 * @brief Arduino main loop function (runs repeatedly forever).
 *
 * Typical pipeline per iteration:
 *  1) Read sensors into SharedState
 *  2) Run controller to decide actuator states
 *  3) Actuator modules apply outputs (often inside controller or module update)
 *  4) Periodically print system state for debugging
 */

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
