/**
 * @file LightSensor.cpp
 * @brief LDR-based light sensing, lamp control and light accumulation.
 * 
 * @details This module reads an LDR on an analog pin and decides whether the environment
 * is dark, using a threshold set by an administrator. It also controls a lamp and tracks
 * how many hours of light is in the greenhouse during a day.
 * 
 * "Hours of light" means that there is enough ambient light or the lamp is turned on.
 * 
 * The daily counter is reset once per day. A day is defined from the time the device is turned
 * on, not actual sunrise.
 * 
 * The lamp is controlled by a strategy where the system compares the accumulated hours of light
 * against a schedule of expected hours of light during a day. It turns on the lamp when
 * the system is behind schedule.
*/

#include "Lightsensor.h"
#include <Arduino.h>
#include <math.h>

/**
 * @name Hardware pins
*/
static const uint8_t LDR_PIN = A0;
static const uint8_t LEDPIN  = D5;

/**
 * @brief Raw ADC threshold for the definition of darkness. 
 *
 * If the analog read function returns a value lower than this,
 * the ambient light is deemed too low.
*/
static int DARK_THRESHOLD = 1000;

/**
 *@brief Minimum time between light samples in milliseconds as to 
 * save on resources.
*/
static const uint32_t SAMPLE_MS = 1000;

/// Internal state
static uint32_t lastSampleMs = 0;
static uint32_t dayStartMs   = 0;
static float hoursToday      = 0.0f;

/**
 * @brief Clamp a floating-point value to a given range.
 *
 * @param x  Value to clamp.
 * @param lo Lower bound.
 * @param hi Upper bound.
 * @return Clamped value in the range [lo, hi].
 */
static float clamp(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

/**
 * @brief Reset daily light accumulation.
 *
 * @details
 * Resets the internal light-hour counter and marks the beginning
 * of a new day. The shared state is updated accordingly.
 *
 * @param now   Current timestamp start of system
 * @param state Shared state
 */

static void resetDay(uint32_t now, SharedState &state) {
  dayStartMs = now;
  hoursToday = 0.0f;
  state.lightHoursToday = 0.0f;
}


/**
 * @brief Start light sensor and lamp.
 * 
 * @details Configures lamp and sensor pin
 * and starts internal timer.
 *
 * Call this function in setup() before lightUpdate().
*/
void lightInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  uint32_t now = millis();
  lastSampleMs = now;
  dayStartMs = now;
  hoursToday = 0.0f;
}

/**
 * @brief Update lamp state and accumulate hours of light.
 * 
 * @details 
 * This function starts by resetting the daily counter every 24 hours. Samples fron the analog pin
 * every (default) 1 seconds. It determines if the light is sufficient and turns on the
 * light if the greenhouse is behind schedule. It counts how many hours of light there has been.
 * 
 * @param state Shared state structure:
 *  - Reads:  `state.targetLightHoursToday`
 *  - Writes: `state.lampOn`, `state.lightHoursToday`
*/
void lightUpdate(SharedState &state) {
  uint32_t now = millis();

  const uint32_t dayLenMs = 24UL * 60UL * 60UL * 1000UL;

  if ((uint32_t)(now - dayStartMs) >= dayLenMs) {
    resetDay(now, state);
  }

  if ((uint32_t)(now - lastSampleMs) < SAMPLE_MS) { /// To avoid reading from the analog pin too much. It is very slow and might throttle program.
    state.lightHoursToday = hoursToday;
    return;
  }

  uint32_t elapsedSampleMs = now - lastSampleMs;
  lastSampleMs = now;

  int raw = analogRead(LDR_PIN);
  bool ambientDark = (raw < DARK_THRESHOLD);

  const float targetHrs  = clamp(state.targetLightHoursToday, 0.0, 24.0); /// Get the target 
  const float elapsedHrs = (now - dayStartMs) / 3600000.0; /// How many hours into the current day are we?
  const float dayLenHrs  = dayLenMs / 3600000.0; /// Day lenght in hours

  bool lampOn = false; /// Turn it off by default, unless it is turned on

  if (targetHrs > 0.0f && hoursToday < targetHrs) {
    float expectedSoFar = targetHrs * (elapsedHrs / dayLenHrs); /// Figuring out how many hours we should've had by now
    bool behindSchedule = (hoursToday + 0.001f < expectedSoFar);
    lampOn = behindSchedule && ambientDark;
  }

  digitalWrite(LEDPIN, lampOn ? HIGH : LOW);
  state.lampOn = lampOn;

  bool effectiveLight = (!ambientDark) || lampOn;
  if (effectiveLight && elapsedSampleMs > 0) {
    hoursToday += (elapsedSampleMs / 1000.0f) / 3600.0f;
    hoursToday = clamp(hoursToday, 0.0f, 24.0f);
  }

  state.lightHoursToday = hoursToday;
}

