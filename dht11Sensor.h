#pragma once
#include "sharedState.h"

/**
 * @file dht11Sensor.h
 * @brief DHT11 sensor module: init + read into SharedState.
 */

/**
 * @brief Initialize the DHT11 hardware/library.
 *
 * Call once in setup().
 */
void dht11Begin();

/**
 * @brief Read temperature + humidity and write results into SharedState.
 *
 * On success:
 *  - state.tempC, state.humidityPct updated
 *  - state.hasDht = true
 *
 * On failure:
 *  - tempC/humidityPct set to NAN
 *  - state.hasDht = false
 *
 * @param state SharedState to update.
 */
void dht11Read(SharedState &state);
