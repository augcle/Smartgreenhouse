/**
 * @file heater.cpp
 * @brief Initializes and toggles the heater
 * 
 * The heater is not actually a heater. It is an LED that represents the heating element. 
 */
#include <Arduino.h>
#include "heater.h"

static const uint8_t HEATER_LED_PIN = D2;  

void heaterBegin() {
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);
}

void heaterSet(SharedState &state, bool on) {
  digitalWrite(HEATER_LED_PIN, on ? HIGH : LOW);
  state.heaterOn = on;
}
