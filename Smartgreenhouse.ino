#include <Arduino.h>
#include "sharedState.h"
#include "dht11Sensor.h"
#include "mister.h"

// Constant and variable definition
unsigned long lastReading = 0;
const unsigned long READING_PERIOD = 3000; 

// Initialize state machine ONLY ONCE and ONLY HERE
SharedState state;

void setup() {
  Serial.begin(115200);
  dht11Begin();
  misterInit();
  lightInit();
}

void loop() {
  unsigned long now = millis();
  if (now - lastReading >= READING_PERIOD){
    lastReading = now;
    // Important dht11 reading
    dht11Read(state);
    
    // Optional dht print statements
    if (state.hasDht) {
      Serial.print("Temp: ");
      Serial.print(state.tempC);
      Serial.print("  Hum: ");
      Serial.println(state.humidityPct);
    } else {
      Serial.println("DHT read failed");
  }
  }
  lightUpdate(state);
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.print("hoursToday=");
    Serial.print(state.lightHoursToday, 2);
    Serial.print(" lampOn=");
    Serial.println(state.lampOn ? "true" : "false");
  }
}
