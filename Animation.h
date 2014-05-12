#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>

#ifndef Animation_h

#define Animation_h

class Animation
{
protected:
    uint8_t led;
    float timer;
    float duration;
    virtual void updateAnimation(float amount) = 0;
public:
    static Adafruit_NeoPixel *neoPixels;
    bool done;
    Animation(uint8_t ledNumber, float duration);
    bool update(float time);
};

#endif
