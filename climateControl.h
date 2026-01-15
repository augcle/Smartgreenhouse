#pragma once
#include "sharedState.h"

/**
 * @file climateControl.h
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

/**
 * @brief Initialize controller timers/state.
 *
 * Call once in setup(). This does not change any outputs by itself.
 */
void climateControlBegin();

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
void climateControlUpdate(SharedState &state);
