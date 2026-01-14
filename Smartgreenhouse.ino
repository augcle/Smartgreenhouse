#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"
#include "mister.h"

// Constant and variable definition
unsigned long lastReading = 0;
const unsigned long READING_PERIOD = 3000; 

// Initialize state machine ONLY ONCE and ONLY HERE
SharedState state;

void setup() {
  Serial.begin(115200);
  dht11Begin();
  misterInit();
  lightInit();
  heaterBegin();
}

void loop() {
  // 1) Read sensors
  dht11Read(state);

  // 2) Run control modules
  lightUpdate(state);
  heaterUpdate(state);

  // 3) (Optional) mister control (leave OFF for now)
  // misterState(false);

  // 4) Debug print
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= PRINT_MS) {
    lastPrint = millis();

    Serial.print("hasDht=");
    Serial.print(state.hasDht ? "true" : "false");

    Serial.print(" tempC=");
    Serial.print(state.tempC, 1);

    Serial.print(" hum=");
    Serial.print(state.humidityPct, 1);

    Serial.print(" lightHoursToday=");
    Serial.print(state.lightHoursToday, 2);

    Serial.print(" lampOn=");
    Serial.print(state.lampOn ? "true" : "false");

    Serial.print(" heaterOn=");
    Serial.println(state.heaterOn ? "true" : "false");
  }
}