/**
 * @file mister.cpp
 * @brief Mister output implementation.
 */

#include "mister.h"

#define MISTERPIN D1

/**
 * Intializing the pin to be used by the mister.
 * Change pin in the define MISTERPIN.
 */
void misterInit() {
    pinMode(MISTERPIN, OUTPUT);
}

/**
 * Turns the mister on or off. Handles unknown states by turning the mister off.  
 * 
 * @param command can be true or false, to turn on or off the mister.
 */
void misterState(bool command) {
    switch (command)
    {
    case true:
        digitalWrite(MISTERPIN, HIGH);
        break;
    case false:
        digitalWrite(MISTERPIN, LOW);
        break;
    default:
        digitalWrite(MISTERPIN, LOW);
        break;
    }
}