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
    
    uint8_t baseR;
    uint8_t baseG;
    uint8_t baseB;
    
    uint8_t fadeR;
    uint8_t fadeG;
    uint8_t fadeB;
public:
    Flash(uint8_t ledNumber, float duration, uint8_t r, uint8_t g, uint8_t b, float delay = 0);
    Flash(uint8_t ledNumber, float duration, uint32_t color, float delay = 0);
    void updateAnimation(float amount);
};

#endif
