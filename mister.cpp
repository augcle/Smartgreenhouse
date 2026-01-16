/**
 * @file mister.cpp
 * @brief Initalises and controls the mister
 * @section Mister_var Internal state varible
 * - MISTER_ACTIVE_LOW: True if the mister control is active-low (LOW = ON, HIGH = OFF).
 * The
 */
#include "mister.h"

#define MISTERPIN D1
static const bool MISTER_ACTIVE_LOW = true;

/**
 * @brief Initializes the mister control pin and sets it to a safe OFF state.
 * @details Configures MISTERPIN as OUTPUT and writes the inactive level,
 *          (OFF) depending on whether the hardware is active: low or active: high.
 */
void misterInit() {
    pinMode(MISTERPIN, OUTPUT);
    digitalWrite(MISTERPIN, MISTER_ACTIVE_LOW ? HIGH : LOW);
}

/**
 * @brief Turns the mister on or off. Handles unknown states by turning the mister off.  
 * 
 * @param on can be true or false, to turn on or off the mister. It is inverted.
 */

void misterState(bool on) {
  if (MISTER_ACTIVE_LOW) {
    digitalWrite(MISTERPIN, on ? LOW : HIGH);   
  } else {
    digitalWrite(MISTERPIN, on ? HIGH : LOW);  
  }


}