#pragma once
#include <Arduino.h>

/**
 * Shared system state
 * Sensor modules write to their own variables
 * Other modules can read from it
 */
struct SharedState {
  // DHT11: Temperature and humidity
  float tempC = NAN;
  float humidityPct = NAN;
  bool  hasDht = false;

  // Light module (hours of light received today + lamp state)
  float lightHoursToday = NAN;
  bool  lampOn = false;

  // Heater module (heat lamp indicator)
  bool  heaterOn = false;
};
