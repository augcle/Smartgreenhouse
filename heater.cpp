/**
 * @file heater.cpp
 * @brief Heater output implementation.
 */
#include <Arduino.h>
#include "heater.h"

/// Output pin used as heater indicator/output (NodeMCU/Wemos pin name)
static const uint8_t HEATER_LED_PIN = D2;

void heaterBegin() {
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);
}

void heaterSet(SharedState &state, bool on) {
  digitalWrite(HEATER_LED_PIN, on ? HIGH : LOW);
  state.heaterOn = on;
}
