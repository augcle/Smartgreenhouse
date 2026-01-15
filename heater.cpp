#include <Arduino.h>
#include "heater.h"

// -------------------- CONFIG --------------------
// Adjust to your project requirement:
static const float MIN_TEMP_C = 27.0f;   // heater ON below this temp

// -------------------- INTERNAL STATE --------------------
static const uint8_t HEATER_LED_PIN = D2;  // your heater indicator LED

// IMPORTANT: This function MUST exist exactly like this,
// otherwise you get: undefined reference to `heaterBegin()`
void heaterBegin() {
  pinMode(HEATER_LED_PIN, OUTPUT);
  digitalWrite(HEATER_LED_PIN, LOW);
}

// Called in loop() after dht11Read(state)
void heaterSet(SharedState &state, bool on) {
  // Apply output
  digitalWrite(HEATER_LED_PIN, on ? HIGH : LOW);

  // Write ONLY what the heater module owns
  state.heaterOn = on;
}
