#pragma once
#include <Arduino.h>

/**
 * @file sharedState.h
 * @brief Shared state container for sensors + actuators.
 *
 * Modules write/read to/from this struct to coordinate behavior.
 */
struct SharedState {
  // --- DHT11: Temperature and humidity ---
  float tempC = NAN;          ///< Latest temperature in °C (NAN if invalid)
  float humidityPct = NAN;    ///< Latest humidity in % (NAN if invalid)
  bool  hasDht = false;       ///< True if last DHT read succeeded

  // --- Light module ---
  float lightHoursToday = 0.0f; ///< Accumulated hours of effective light today
  bool  lampOn = false;        ///< True when lamp LED output is ON

  uint32_t dayStartMs = 0;

  // --- Actuator states ---
  bool heaterOn = false;      ///< True when heater output is ON
  bool misterOn = false;      ///< True when mister output is ON

  // --- Targets (set by main / controller) ---
  float targetTempC = 27.0f;        ///< Desired temperature (°C)
  float targetHumidityPct = 65.0f;  ///< Desired humidity (%)
  float targetLightHoursToday = 6.0f;      ///< Desired light hours per day
};
