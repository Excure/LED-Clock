#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>

#ifndef Flash_h

#define Flash_h

class Flash
{
private:
    BackEase ease;
    uint8_t led;
    float timer;
    float duration;
    uint8_t fadeR;
    uint8_t fadeG;
    uint8_t fadeB;
public:
    bool done;
    Flash(uint8_t ledd, float dur, uint8_t r, uint8_t g, uint8_t b);
    static Adafruit_NeoPixel *neoPixels;
    bool update(float time);
};

#endif
