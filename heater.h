#pragma once
#include "sharedState.h"

/**
 * @file heater.h
 * @brief Heater output module
 * 
 * The heater is not actually a heater. It is an LED that represents the heating element. 
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
