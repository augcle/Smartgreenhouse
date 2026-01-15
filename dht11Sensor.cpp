/**
 * @file dht11Sensor.cpp
 * @brief DHT11 implementation.
 */
#include "dht11Sensor.h"
#include <DHT.h> // Adafruit DHT sensor library

#define DHTPIN  D7
#define DHTTYPE DHT11

/// Internal DHT driver instance (module-owned)
static DHT dht(DHTPIN, DHTTYPE);

void dht11Begin() {
  dht.begin();
}

void dht11Read(SharedState &state) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // DHT library returns NaN on failure
  if (isnan(h) || isnan(t)) {
    state.hasDht = false;
    state.tempC = NAN;
    state.humidityPct = NAN;
    return;
  }

  state.humidityPct = h;
  state.tempC = t;
  state.hasDht = true;
}
