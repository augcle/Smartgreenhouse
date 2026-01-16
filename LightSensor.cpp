/**
 * @file LightSensor.cpp
 * @brief Light sensor (LDR) logic + lamp + daily light counter.
 * @details The code main job is to read/sensor how much sun light the plant need for optimal growth, the code uses the photosensor to get data on how sun light the plant get then with at set threshold either turns on or off the lamp for the plant for light
 * 
 *  * @section light_vars Internal state variables
 * 
 * - LDR_PIN:      ADC pin for the LDR divider (A0)
 * - LEDPIN:       Output pin for the lamp (D3)
 * - threshold:    Light threshold to turn on LED/Lamp
 * - TARGET_HOURS: Daily target light hours
 * - lastMs:       Last update timestamp 
 * - dayStartMs:   Start of day timestamp 
 * - hoursToday:   Accumulated effective light hours today
 */

#include "LightSensor.h"

static const uint8_t LDR_PIN = A0; 

static const uint8_t LEDPIN  = D3; 

static int threshold = 950; 

static const float TARGET_HOURS = 6.0f; 

static unsigned long lastMs = 0; 

static unsigned long dayStartMs = 0; 
static float hoursToday = 0.0f; 

/**
 * @brief Clamp a floating-point value to a closed interval.
 *
 * @details Returns:
 * - lo if x < lo
 * - hi if x > hi
 * - x otherwise
 *
 * @param x  Input value.
 * @param lo Lower bound (inclusive).
 * @param hi Upper bound (inclusive).
 * @return Clamped value in the range [lo, hi].
 */

static float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

/**
 * @brief Initialize the light module (lamp output + daily tracking timer).
 *
 * @details
 * Configures the lamp output pin and resets the internal timing state used to
 * accumulate "effective light" hours for the current day.
 *
 * - Sets LEDPIN as OUTPUT and turns the lamp OFF.
 * - Initializes timing variables (lastMs, dayStartMs) to millis().
 * - Resets hoursToday to 0.0.
 *
 * @note Call this once from setup() before calling lightUpdate().
 */
void lightInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  unsigned long now = millis();
  lastMs = now;
  dayStartMs = now;
  hoursToday = 0.0f;
}

/**
 * @brief Initialize the LED sturn on and off method, and the intialize the clock tracking time
 * 
 * @details 
 * the method for turning on the lamp, has 2 requirments:
 * - First requirment is the raw data from dht is has to be lower than threshold, which means if the light is lower than the threshold you want than the first requirment is met.
 * - Second requirment is before turning on the lamp, the code checks if how long the plants has gotten light today, if the hours required isn't reached first then the lamp turns on.
 * 
 * @param state Sharedstate to update and communicate with overall main code
 */
void lightUpdate(SharedState &state) {
  unsigned long now = millis();

  resetDayIfNeeded(now);

  unsigned long dtMs = (unsigned long)(now - lastMs);
  lastMs = now;

  int raw = analogRead(LDR_PIN);

  bool dark = (raw < threshold);

  bool shouldLampOn = dark && (hoursToday < TARGET_HOURS);

  digitalWrite(LEDPIN, shouldLampOn ? HIGH : LOW);

  bool effectiveLight = (!dark) || shouldLampOn;

  if (effectiveLight && dtMs > 0) {
    hoursToday += (dtMs / 1000.0f) / 3600.0f;
    hoursToday = clampf(hoursToday, 0.0f, 24.0f);
  }

  state.lightHoursToday = hoursToday;       
  state.lampOn = shouldLampOn;     
}
