#pragma once
#ifndef MISTER_H
#define MISTER_H

#include <Arduino.h> // Might be unnecessary since it is called in the main file

void misterInit(); /// Call the function with no arguments. It initializes the pin to be used for the mister
void misterState(bool command); /// Call the function with a simple 'true' or 'false' command to turn the mister on or off

#endif