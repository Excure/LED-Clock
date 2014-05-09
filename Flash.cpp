#include "Flash.h"

Flash::Flash(uint8_t ledNumber, float duration, uint8_t r, uint8_t g, uint8_t b, float delay) : Animation(ledNumber, duration, delay)
{
    uint32_t baseColor = neoPixels->getPixelColor(ledNumber);
    
    baseR = (uint8_t)(baseColor >> 16),
    baseG = (uint8_t)(baseColor >>  8),
    baseB = (uint8_t)baseColor;
    
    fadeR = r;
    fadeG = g;
    fadeB = b;
    
    ease.setDuration(0.5);
    ease.setTotalChangeInPosition(1);
}

Flash::Flash(uint8_t ledNumber, float duration, uint32_t color, float delay) : Animation(ledNumber, duration, delay)
{
    uint32_t baseColor = neoPixels->getPixelColor(ledNumber);
    
    baseR = (uint8_t)(baseColor >> 16),
    baseG = (uint8_t)(baseColor >>  8),
    baseB = (uint8_t)baseColor;
    
    fadeR = (uint8_t)(color >> 16),
    fadeG = (uint8_t)(color >>  8),
    fadeB = (uint8_t)color;
    
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
    
    float inverseScaledAmount = 1.0 - scaledAmount;
    
    neoPixels->setPixelColor(led, scaledAmount * fadeR + inverseScaledAmount * baseR, scaledAmount * fadeG  + inverseScaledAmount * baseG, scaledAmount * fadeB + inverseScaledAmount * baseB);
}