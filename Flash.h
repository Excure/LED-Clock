#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "Animation.h"

#ifndef Flash_h

#define Flash_h

class Flash : public Animation
{
private:
    QuinticEase ease;
    uint8_t fadeR;
    uint8_t fadeG;
    uint8_t fadeB;
public:
    Flash(uint8_t ledNumber, float duration, uint8_t r, uint8_t g, uint8_t b);
    void updateAnimation(float amount);
};

#endif
