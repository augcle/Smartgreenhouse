/**
 * @file mister.cpp
 * @brief Initalises and controls the mister
 * 
 * The
 */

#include "mister.h"

#define MISTERPIN D1
static const bool MISTER_ACTIVE_LOW = true;

/**
 * @brief Initialize the mister output pin.
 *
 * Call once in setup().
 */
void misterInit() {
    pinMode(MISTERPIN, OUTPUT);

    // Ensure mister starts off
    digitalWrite(MISTERPIN, MISTER_ACTIVE_LOW ? HIGH : LOW);
}

/**
 * Turns the mister on or off. Handles unknown states by turning the mister off.  
 * 
 * @param on can be true or false, to turn on or off the mister. It is inverted.
 */

void misterState(bool on) {
  if (MISTER_ACTIVE_LOW) {
    digitalWrite(MISTERPIN, on ? LOW : HIGH);   // inverted
  } else {
    digitalWrite(MISTERPIN, on ? HIGH : LOW);  // normal
  }


}