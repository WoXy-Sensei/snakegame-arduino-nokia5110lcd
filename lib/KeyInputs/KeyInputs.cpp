#include "Arduino.h"
#include "KeyInputs.h"

void KeyInputs::ReadKeys()
{
    up = !digitalRead(A0);
    down = !digitalRead(A1);
    left = !digitalRead(A2);
    right = !digitalRead(A3);
    if (up)
    {
        keyPressed = 'U';
    }
    else if (down)
    {
        keyPressed = 'D';
    }
    else if (left)
    {
        keyPressed= 'L';
    }
    else if (right)
    {
        keyPressed = 'R';
    }
    else
    {
        keyPressed = ' ';
    }
}


void KeyInputs::Reset()
{
    keyPressed = ' ';
}

