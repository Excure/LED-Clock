#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "Animation.h"

#ifndef FadeIn_h

#define FadeIn_h

class FadeIn : public Animation
{
private:
    QuinticEase ease;
    
    uint8_t baseR;
    uint8_t baseG;
    uint8_t baseB;
    
    uint8_t fadeR;
    uint8_t fadeG;
    uint8_t fadeB;
public:
    FadeIn(uint8_t ledNumber, float duration, uint32_t color, float delay = 0);
    void updateAnimation(float amount);
};

#endif
