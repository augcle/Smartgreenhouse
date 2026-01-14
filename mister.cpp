#include "mister.h"

#define MISTERPIN D1

pinMode(MISTERPIN, OUTPUT)

misterState(bool command) {
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