#include "LightSensor.h"

// Matcher din fungerende test
static const uint8_t LDR_PIN = A0;
static const uint8_t LEDPIN  = D2;

// Kalibrering
static int threshold = 950;
static const float TARGET_HOURS = 6.0f;

// Runtime (internt i modulet)
static unsigned long lastMs = 0;
static unsigned long dayStartMs = 0;
static float hoursToday = 0.0f;

static float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static void resetDayIfNeeded(unsigned long now) {
  const unsigned long DAY_MS = 24UL * 60UL * 60UL * 1000UL;
  if ((unsigned long)(now - dayStartMs) >= DAY_MS) {
    dayStartMs = now;
    hoursToday = 0.0f;
  }
}

void lightInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  unsigned long now = millis();
  lastMs = now;
  dayStartMs = now;
  hoursToday = 0.0f;
}

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

  // kun 1 float + 1 bool ud til resten af systemet
  state.light = hoursToday;
  state.lampOn = shouldLampOn;
}
