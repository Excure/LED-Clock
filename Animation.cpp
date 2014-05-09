#include "Animation.h"

Adafruit_NeoPixel* Animation::neoPixels = 0;

Animation::Animation(uint8_t ledNumber, float duration, float delay)
{
    this->delay = delay;
    led = ledNumber;
    timer = 0;
    this->duration = duration;
    done = false;
}

bool Animation::update(float time)
{
    if (!done)
    {
        if (delay > 0)
        {
            delay -= time;
            if (delay <= 0)
            {
                delay = 0;
            }
            else
            {
                return true;
            }
        }
        
        timer += time;
        if (timer >= duration)
        {
            done = true;
            updateAnimation(1.0);
            return false;
        }
        else
        {
            float amount = timer / duration;
            updateAnimation(amount);
            return true;
        }
    }
    else
    {
        return false;
    }
}