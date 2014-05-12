#include "FadeIn.h"

FadeIn::FadeIn(uint8_t ledNumber, float duration, uint32_t color) : Animation(ledNumber, duration)
{
    uint32_t baseColor = neoPixels->getPixelColor(ledNumber);
    
    baseR = (uint8_t)(baseColor >> 16),
    baseG = (uint8_t)(baseColor >>  8),
    baseB = (uint8_t)baseColor;
    
    fadeR = (uint8_t)(color >> 16),
    fadeG = (uint8_t)(color >>  8),
    fadeB = (uint8_t)color;
    
    ease.setDuration(1.0);
    ease.setTotalChangeInPosition(1);
}

void FadeIn::updateAnimation(float amount)
{
    float scaledAmount;
    
    scaledAmount = ease.easeIn(amount);
    
    float inverseScaledAmount = 1.0 - scaledAmount;
    
    neoPixels->setPixelColor(led, scaledAmount * fadeR + inverseScaledAmount * baseR, scaledAmount * fadeG  + inverseScaledAmount * baseG, scaledAmount * fadeB + inverseScaledAmount * baseB);
}