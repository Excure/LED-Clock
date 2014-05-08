#include "Flash.h"

Flash::Flash(uint8_t ledNumber, float duration, uint8_t r, uint8_t g, uint8_t b) : Animation(ledNumber, duration)
{
    fadeR = r;
    fadeG = g;
    fadeB = b;
    ease.setDuration(0.5);
    ease.setTotalChangeInPosition(1);
}

void Flash::updateAnimation(float amount)
{
    float scaledAmount;

    if (amount <= 0.5)
        scaledAmount = ease.easeOut(amount);
    else
        scaledAmount = 1.0 - ease.easeIn(amount - 0.5);
    
    neoPixels->setPixelColor(led, scaledAmount * fadeR, scaledAmount * fadeG, scaledAmount * fadeB);
}