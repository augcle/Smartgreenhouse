/**
 * @file sharedState.h
 */

#pragma once
#include <Arduino.h>


/// @brief Shared runtime state for the greenhouse.
/// @details Holds latest sensor readings, actuator states, and target setpoints.        
struct SharedState {
  float tempC;              ///< Latest temperature in °C.
  float humidityPct;        ///< Latest humidity in %.
  bool  hasDht;             ///< True if the last DHT read was valid.

  float lightHoursToday;    ///< Accumulated light hours today (Clock).
  bool  lampOn;             ///< True if the lamp is currently ON.

  bool  heaterOn;           ///< True if the heater output is ON.
  bool  misterOn;           ///< True if the mister output is ON.

  float targetTempC;        ///< Desired temperature setpoint (°C).
  float targetHumidityPct;  ///< Desired humidity setpoint (%).
};
