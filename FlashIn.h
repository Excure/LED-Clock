#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "Animation.h"

#ifndef FlashIn_h

#define FlashIn_h

class FlashIn : public Animation
{
private:
    QuinticEase ease;
    
    uint8_t baseR;
    uint8_t baseG;
    uint8_t baseB;
    
    uint8_t flashR;
    uint8_t flashG;
    uint8_t flashB;
    
    uint8_t targetR;
    uint8_t targetG;
    uint8_t targetB;
public:
    FlashIn(uint8_t ledNumber, float duration, uint8_t flashR, uint8_t flashG, uint8_t flashB, uint8_t targetR, uint8_t targetG, uint8_t targetB);
    FlashIn(uint8_t ledNumber, float duration, uint32_t flashColor, uint32_t targetColor);
    void updateAnimation(float amount);
};

#endif
