#include "LightSensor.h"

// Hardware pins (matches your working test setup)
// LDR voltage divider output is connected to A0 (ESP ADC: 0..1023)
static const uint8_t LDR_PIN = A0;

// Plant lamp indicator LED pin (NodeMCU/Wemos style pin name)
static const uint8_t LEDPIN  = D2;

// --- Calibration constants ---
// Threshold used to decide "dark" vs "bright".
// If raw < threshold => dark. Adjust based on your measured raw values.
static int threshold = 950;

// Daily target light amount for the plant (in hours).
// When the plant has received this many hours today, we stop turning on the lamp.
static const float TARGET_HOURS = 6.0f;

// --- Internal runtime variables (kept inside this module) ---
// Used to compute delta time between updates (non-blocking timing via millis()).
static unsigned long lastMs = 0;

// Used to reset the daily counter every 24 hours.
static unsigned long dayStartMs = 0;

// Accumulated light time received today (hours).
static float hoursToday = 0.0f;

// Clamp helper to keep values within a safe range.
static float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

// Resets the daily counter once every 24 hours.
// This is a simple "rolling 24h window from boot" approach.
static void resetDayIfNeeded(unsigned long now) {
  const unsigned long DAY_MS = 24UL * 60UL * 60UL * 1000UL; // 24 hours in ms

  // If more than 24h has passed since last "day start", start a new day
  if ((unsigned long)(now - dayStartMs) >= DAY_MS) {
    dayStartMs = now;
    hoursToday = 0.0f;
  }
}

// Initializes the light module:
// - sets LED pin as output and turns it off
// - initializes timing variables
void lightInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  unsigned long now = millis();
  lastMs = now;
  dayStartMs = now;
  hoursToday = 0.0f;
}

// Main update function (call repeatedly from loop()):
// - reads the LDR analog value
// - decides whether it is dark
// - turns lamp LED on/off depending on darkness and remaining target hours
// - accumulates how many hours of light the plant has received today
// - writes only 1 float + 1 bool to SharedState:
//     state.light  = hoursToday
//     state.lampOn = lamp LED status
void lightUpdate(SharedState &state) {
  unsigned long now = millis();

  // Reset daily light counter if a new day started
  resetDayIfNeeded(now);

  // Compute elapsed time since last update (ms)
  unsigned long dtMs = (unsigned long)(now - lastMs);
  lastMs = now;

  // Read raw light value from ADC (0..1023)
  int raw = analogRead(LDR_PIN);

  // Determine if it is dark based on threshold
  bool dark = (raw < threshold);

  // Lamp should only turn on if:
  // 1) It's dark, AND
  // 2) The plant has not yet reached the daily light target
  bool shouldLampOn = dark && (hoursToday < TARGET_HOURS);

  // Apply lamp state to physical LED
  digitalWrite(LEDPIN, shouldLampOn ? HIGH : LOW);

  // Count "effective light time":
  // The plant receives light if:
  // - ambient is bright (!dark), OR
  // - our lamp is on (shouldLampOn)
  bool effectiveLight = (!dark) || shouldLampOn;

  // If the plant is receiving light, accumulate time into hoursToday
  // dtMs -> seconds -> hours
  if (effectiveLight && dtMs > 0) {
    hoursToday += (dtMs / 1000.0f) / 3600.0f;
    hoursToday = clampf(hoursToday, 0.0f, 24.0f);
  }

  // Export only the required outputs to the shared system state
  // (1 float + 1 bool)
  state.lightHoursToday = hoursToday;       // "hours of light received today"
  state.lampOn = shouldLampOn;     // lamp LED ON/OFF
}
