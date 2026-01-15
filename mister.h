#pragma once
#include <Arduino.h>

/**
 * @file mister.h
 * @brief Mister output module (on/off).
 */

/**
 * @brief Initialize the mister output pin.
 *
 * Call once in setup().
 */
void misterInit();

/**
 * @brief Turn mister output ON/OFF.
 *
 * @param command True = ON, False = OFF.
 */
void misterState(bool command);
