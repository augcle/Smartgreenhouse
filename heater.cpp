/**
 * @file heater.cpp
 * @brief Heater output implementation.
 * @details The code states which pin is reserved for the heaping Lamp, and declares whether or not the LED is on
 * 
 * @section Heater_var Internal state varibles
 * - HEATER_LED_PIN: Hardware pin for LED/heater lamp
 */
#include <Arduino.h>
#include "heater.h"

static const uint8_t HEATER_LED_PIN = D2;

/**
 * @brief Initialize the heater output pin.
 * 
 * @details
 * Configures the heater control/indicator pin as OUTPUT and ensures the heater
 * starts in a safe OFF state.
 *
 * - pinMode(HEATER_LED_PIN, OUTPUT)
 * - digitalWrite(HEATER_LED_PIN, LOW)
 *
 * @note Call once in setup() before using heaterSet().
 */

void heaterBegin() {
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);
}

/**
 * @brief Turn the output ON or OFF
 * 
 * @details
 * Writes the heater pin (HEATER_LED_PIN) and updates the shared state so the rest
 * of the program knows the current heater status.
 * 
 * @param state SharedState to update
 * @param on True to turn the heater ON, false to turn it OFF
 */

void heaterSet(SharedState &state, bool on) {
  digitalWrite(HEATER_LED_PIN, on ? HIGH : LOW);
  state.heaterOn = on;
}
