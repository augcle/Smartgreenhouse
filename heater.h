#pragma once
#include "sharedState.h"

/**
 * @file heater.h
 * @brief Heater output module (LED / relay abstraction).
 */

/**
 * @brief Initialize heater output pin(s).
 *
 * Call once in setup().
 */
void heaterBegin();

/**
 * @brief Turn heater output ON/OFF and update SharedState.
 *
 * @param state SharedState to update (writes state.heaterOn only).
 * @param on True to turn heater on, false to turn it off.
 */
void heaterSet(SharedState &state, bool on);
