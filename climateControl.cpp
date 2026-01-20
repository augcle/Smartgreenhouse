#include <Arduino.h>
#include "climateControl.h"
#include "heater.h"
#include "mister.h"

/**
 * @file climateControl.cpp
 * @brief Climate controller: applies control logic (heater + mister) from SharedState sensor values/targets.
 *
 * This module implements:
 *  - Temperature control using hysteresis (prevents rapid toggling).
 *  - Humidity control using hysteresis + safety timers (max ON time + minimum OFF backoff).
 *
 * Typical use:
 *  - call climateControlBegin() once in setup()
 *  - call climateControlUpdate(state) repeatedly in loop()
 */

//Allowed temperature hysteriria
static const float TEMP_HYST = 0.5;
static const float HUM_HYST = 3.0;

static const uint32_t MIST_MAX_ON_MS = 20 * 1000; // max continous on
static const uint32_t MIST_MIN_OFF_MS = 10 * 1000; // minimum backoff interval

// interval timer
static uint32_t mistOnSinceMs = 0;
static uint32_t mistOffSinceMs = 0;

/**
 * @brief Initialize controller timers/state.
 *
   * Call once in setup(). This does not change any outputs by itself.
 */
void climateControlBegin() {
  mistOnSinceMs = 0;
  mistOffSinceMs = millis();
}

/**
 * @brief Update climate control (heater + mister) based on sensor readings and targets.
 *
 * Inputs (read from @p state):
 *  - state.hasDht, state.tempC, state.humidityPct
 *  - state.targetTempC, state.targetHumidityPct
 *  - state.heaterOn, state.misterOn (used for hysteresis decisions)
 *
 * Outputs (written to @p state and hardware):
 *  - Heater output via heaterSet(state, on) (also updates state.heaterOn)
 *  - Mister output via misterState(on) and state.misterOn
 *
 * Fail-safe:
 *  - If state.hasDht is false, heater and mister are turned OFF.
 *
 * @param state Shared state containing latest sensor values, setpoints, and actuator states.
 */
void climateControlUpdate(SharedState &state) {
  // If sensor invalid, fail safe: turn things off
  if (!state.hasDht) {
    heaterSet(state, false);
    misterState(false);
    state.misterOn = false;
    return;
  }

  const float t = state.tempC;
  const float h = state.humidityPct;

  // Temperature Control
  // turn on if too cold, turn off if warm enough
  if (!state.heaterOn && t <= (state.targetTempC - TEMP_HYST)) {
    heaterSet(state, true);
  } else if (state.heaterOn && t >= (state.targetTempC + TEMP_HYST)) {
    heaterSet(state, false);
  }

  // Humidity Control
  const uint32_t now = millis();

  // Track timers
  // Track how long the mister has been off or on for
  if (state.misterOn) {
    if (mistOnSinceMs == 0) {
      mistOnSinceMs = now;
    }

  mistOffSinceMs = 0;

  } else {
    if (mistOffSinceMs == 0) {
      mistOffSinceMs = now;
    }

  mistOnSinceMs = 0;
  }

  // Safety. If mister has been turned on for too long, turn it off.
  if (state.misterOn && mistOnSinceMs != 0 && (now - mistOnSinceMs) >= MIST_MAX_ON_MS) {
    misterState(false);
    state.misterOn = false;
    return;
  }

  const bool canTurnOn = (mistOffSinceMs != 0) && ((now - mistOffSinceMs) >= MIST_MIN_OFF_MS);

  // Finally, turn the mister on or off 
  if (!state.misterOn && canTurnOn && h <= (state.targetHumidityPct - HUM_HYST)) {
    misterState(true);
    state.misterOn = true;
  } else if (state.misterOn && h >= (state.targetHumidityPct + HUM_HYST)) {
    misterState(false);
    state.misterOn = false;
  }



}