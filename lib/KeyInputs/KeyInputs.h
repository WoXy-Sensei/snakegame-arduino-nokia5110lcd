#include "Vector2.h"

class KeyInputs {

    int up;
    int down;
    int left;
    int right;


  public: 
    char keyPressed;
    void ReadKeys();
    void Reset();
};