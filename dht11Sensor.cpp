#include "dht11Sensor.h"
#include <DHT.h>

#define DHTPIN 2       // adjust for your ESP8266
#define DHTTYPE DHT11

static DHT dht(DHTPIN, DHTTYPE);

bool dht11Begin() {
  dht.begin();
  return true;
}

bool dht11Read(SharedState &state) {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    state.hasDht = false;
    return false;
  }

  state.tempC = t;
  state.humidityPct = h;
  state.hasDht = true;
  return true;
}
