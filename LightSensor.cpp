#include "Lightsensor.h"
#include <Arduino.h>
#include <math.h>

// Hardware pins
static const uint8_t LDR_PIN = A0;
static const uint8_t LEDPIN  = D5;

// Calibration (tune these)
static int DARK_THRESHOLD = 1000;   // raw ADC threshold for "dark"

// Update cadence inside lightUpdate (so you can call it often)
static const uint32_t SAMPLE_MS = 1000; // sample each 1s (cheap)

// Internal state
static uint32_t lastSampleMs = 0;
static uint32_t dayStartMs   = 0;
static float hoursToday      = 0.0f;

// Choose policy
enum LightPolicy {
  POLICY_DEADLINE = 0,   // your idea: wait until "must"
  POLICY_PACED    = 1    // recommended: keep pace through day
};
static const LightPolicy POLICY = POLICY_PACED;

// Helpers
static float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static void resetDay(uint32_t now, SharedState &state) {
  dayStartMs = now;
  hoursToday = 0.0f;
  state.lightHoursToday = 0.0f;
}

void lightInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  uint32_t now = millis();
  lastSampleMs = now;
  dayStartMs = now;
  hoursToday = 0.0f;
}

void lightUpdate(SharedState &state) {
  uint32_t now = millis();

  // Day length: use state if you add it, otherwise 24h default here
  const uint32_t dayLenMs = 24UL * 60UL * 60UL * 1000UL;

  // Reset day if needed
  if ((uint32_t)(now - dayStartMs) >= dayLenMs) {
    resetDay(now, state);
  }

  // Throttle sampling so calling lightUpdate() often is safe
  if ((uint32_t)(now - lastSampleMs) < SAMPLE_MS) {
    // still mirror state out
    state.lightHoursToday = hoursToday;
    return;
  }

  uint32_t dtMs = now - lastSampleMs;
  lastSampleMs = now;

  int raw = analogRead(LDR_PIN);
  bool ambientDark = (raw < DARK_THRESHOLD);

  // Determine whether we should turn lamp on
  const float targetHrs = clampf(state.targetLightHoursToday, 0.0f, 24.0f);
  const float elapsedHrs = (now - dayStartMs) / 3600000.0f;
  const float dayLenHrs  = dayLenMs / 3600000.0f;
  const float remainingHrs = clampf(dayLenHrs - elapsedHrs, 0.0f, dayLenHrs);

  bool wantLamp = false;

  if (targetHrs <= 0.0f) {
    wantLamp = false;
  } else if (hoursToday >= targetHrs) {
    wantLamp = false;
  } else {
    if (POLICY == POLICY_DEADLINE) {
      // Your idea: wait until remaining time equals time needed
      float neededHrs = targetHrs - hoursToday;
      wantLamp = (remainingHrs <= neededHrs);
    } else {
      // Paced: by now, you "should" have earned this many hours
      float expectedSoFar = targetHrs * (elapsedHrs / dayLenHrs);
      // If you're behind, supplement now
      wantLamp = (hoursToday + 0.001f < expectedSoFar);
    }
  }

  // Final: lamp only helps if ambient is dark (no reason to add lamp in bright light)
  bool lampOn = wantLamp && ambientDark;

  digitalWrite(LEDPIN, lampOn ? HIGH : LOW);
  state.lampOn = lampOn;

  // Effective light is: sun (not dark) OR lamp
  bool effectiveLight = (!ambientDark) || lampOn;
  if (effectiveLight && dtMs > 0) {
    hoursToday += (dtMs / 1000.0f) / 3600.0f;
    hoursToday = clampf(hoursToday, 0.0f, 24.0f);
  }

  state.lightHoursToday = hoursToday;
}
