#include "FlashIn.h"

FlashIn::FlashIn(uint8_t ledNumber, float duration, uint8_t flashR, uint8_t flashG, uint8_t flashB, uint8_t targetR, uint8_t targetG, uint8_t targetB) : Animation(ledNumber, duration)
{
    uint32_t baseColor = neoPixels->getPixelColor(ledNumber);
    
    baseR = (uint8_t)(baseColor >> 16),
    baseG = (uint8_t)(baseColor >>  8),
    baseB = (uint8_t)baseColor;
    
    this->flashR = flashR;
    this->flashG = flashG;
    this->flashB = flashB;
    
    this->targetR = targetR;
    this->targetG = targetG;
    this->targetB = targetB;
    
    ease.setDuration(0.5);
    ease.setTotalChangeInPosition(1);
}

FlashIn::FlashIn(uint8_t ledNumber, float duration, uint32_t flashColor, uint32_t targetColor) : Animation(ledNumber, duration)
{
    uint32_t baseColor = neoPixels->getPixelColor(ledNumber);
    
    baseR = (uint8_t)(baseColor >> 16),
    baseG = (uint8_t)(baseColor >>  8),
    baseB = (uint8_t)baseColor;
    
    flashR = (uint8_t)(flashColor >> 16),
    flashG = (uint8_t)(flashColor >>  8),
    flashB = (uint8_t)flashColor;

    targetR = (uint8_t)(targetColor >> 16),
    targetG = (uint8_t)(targetColor >>  8),
    targetB = (uint8_t)targetColor;
    
    ease.setDuration(0.5);
    ease.setTotalChangeInPosition(1);
}

void FlashIn::updateAnimation(float amount)
{
    float scaledAmount;
    float inverseScaledAmount = 0;
    float targetScaledAmount = 0;
    
    if (amount <= 0.5)
    {
        scaledAmount = ease.easeOut(amount);
        inverseScaledAmount = 1.0 - scaledAmount;
    }
    else
    {
        scaledAmount = 1.0 - ease.easeIn(amount - 0.5);
        targetScaledAmount = 1.0 - scaledAmount;
    }
    
    neoPixels->setPixelColor(led, inverseScaledAmount * baseR + scaledAmount * flashR + targetScaledAmount * targetR, inverseScaledAmount * baseG + scaledAmount * flashG  + targetScaledAmount * targetG, inverseScaledAmount * baseB + scaledAmount * flashB + targetScaledAmount * targetB);
}