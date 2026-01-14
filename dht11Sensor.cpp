#include "dht11Sensor.h"
#include <DHT.h> // from the DHT sensor library by Adafruit (verbatim)

#define DHTPIN D7
#define DHTTYPE DHT11 // library includes many types, so definition is necessary
static DHT dht(DHTPIN, DHTTYPE); 


void dht11Begin() {
  dht.begin(); // start the sensor
}

void dht11Read(SharedState &state) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Error case. DHT uses NaN to signal failure
  if (isnan(h) || isnan(t)) { 
    state.hasDht = false; // says the reading failed
    state.tempC = NAN;
    state.humidityPct = NAN;
    return;
  }

  // Success case
  state.humidityPct = h;
  state.tempC = t;
  state.hasDht = true;
}
