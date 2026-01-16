/**
 * @file dht11Sensor.cpp
 * @brief Implementing the DHT11 sensor
 * 
 * The file handles the reading from the DHT11 sensor and updating the associated sharedState
 */
#include "dht11Sensor.h"
#include <DHT.h> // Adafruit DHT sensor library

/// Pin used to on the microcontroller for signal to the sensor
#define DHTPIN  D7
/// Type of sensor module. Used to correctly read information using the pre-build library.
#define DHTTYPE DHT11

/// Internal DHT driver instance (module-owned)
static DHT dht(DHTPIN, DHTTYPE);

/**
 * @brief Initialize the DHT11 hardware/library.
 *
 * Call once in setup().
 */
void dht11Begin() {
  dht.begin();
}

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
void dht11Read(SharedState &state) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
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
