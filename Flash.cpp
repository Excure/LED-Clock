#include "Flash.h"

Adafruit_NeoPixel* Flash::neoPixels = 0;

Flash::Flash(uint8_t ledd, float dur, uint8_t r, uint8_t g, uint8_t b)
{
    led = ledd;
    timer = 0;
    duration = dur;
    fadeR = r;
    fadeG = g;
    fadeB = b;
    done = false;
    
    ease.setDuration(0.5);
    ease.setTotalChangeInPosition(1);
}

bool Flash::update(float time)
{
    if (!done)
    {
        timer += time;
        if (timer >= duration)
        {
            neoPixels->setPixelColor(led, 0, 0, 0);
            done = true;
            return false;
        }
        else
        {
            double amount = timer / duration;
            
            double scaledAmount;

            if (amount <= 0.5)
                scaledAmount = ease.easeOut(amount);
            else
                scaledAmount = 1.0 - ease.easeIn(amount - 0.5);

            double inverseScaledAmount = constrain(1.0 - scaledAmount, 0.0, 100.0);
            
            neoPixels->setPixelColor(led, scaledAmount * fadeR, scaledAmount * fadeG, scaledAmount * fadeB);
            return true;
        }
    }
    else
    {
        return false;
    }
}