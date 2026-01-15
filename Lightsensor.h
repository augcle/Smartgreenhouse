#pragma once
#include <Arduino.h>
#include "sharedState.h"

/**
 * @file LightSensor.h
 * @brief Light sensor module (LDR) + lamp control + daily light counter.
 */

/**
 * @brief Initialize light sensor module (pins + timing).
 *
 * Call once in setup().
 */
void lightInit();

/**
 * @brief Update light logic and write results to SharedState.
 *
 * Writes:
 *  - state.lightHoursToday
 *  - state.lampOn
 *
 * Call repeatedly in loop().
 *
 * @param state SharedState to update.
 */
void lightUpdate(SharedState &state);
