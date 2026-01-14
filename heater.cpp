#include <Arduino.h>
#include "heater.h"

// -------------------- CONFIG --------------------
// Adjust to your project requirement:
static const float MIN_TEMP_C = 27.0f;   // heater ON below this temp

// -------------------- INTERNAL STATE --------------------
static const uint8_t HEATER_LED_PIN = D2;  // your heater indicator LED
static bool gHeaterOn = false;

// IMPORTANT: This function MUST exist exactly like this,
// otherwise you get: undefined reference to `heaterBegin()`
void heaterBegin() {
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);
  gHeaterOn = false;
}

// Called in loop() after dht11Read(state)
void heaterUpdate(SharedState &state) {
  // Fail-safe: if temperature is not valid/available, heater OFF
  if (!state.hasDht) {
    gHeaterOn = false;
    digitalWrite(HEATER_LED_PIN, LOW);
    state.heaterOn = false;
    return;
  }

  // Control logic
  gHeaterOn = (state.tempC < MIN_TEMP_C);

  // Apply output
  digitalWrite(HEATER_LED_PIN, gHeaterOn ? HIGH : LOW);

  // Write ONLY what the heater module owns
  state.heaterOn = gHeaterOn;
}
