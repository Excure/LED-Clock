#include "Arduino.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "FlashIn.h"
#include "StandardCplusplus.h"
#include <vector>
#include <MemoryFree.h>
#include "FadeIn.h"

#define TimeOffset 0
#define BaseBrightness 127
#define HighlightBrightness 255
#define UpdateSkips 8
#define LEDStartDelay 3
#define MinuteAnimationDelay 0.06
#define ReverseMinuteAnimationDelay 0.03

RTC_DS3231 RTC;
LiquidCrystal lcd(11, 10, 9, 12, 4, 3);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60 + 24, 6, NEO_GRB + NEO_KHZ800);

uint8_t updateSkipCounter = 0;
uint8_t LEDStartDelayCounter = 0;
boolean LEDsEnabled = false;

unsigned long millisPerSecond = 0;
uint16_t updatesPerSecond = 0;

unsigned long lastUpdateMillis = 0;
unsigned long lastSecondMillis = 0;
uint32_t previousUnixTime = 0;

uint32_t zeroColor;
uint32_t quarterDayColor;
uint32_t hourColor;
uint32_t minuteColor;
uint32_t minuteAnimationColor;
uint32_t hourAnimationColor;
uint32_t quarterHourColor;
uint32_t fiveMinuteColor;
uint32_t secondColor;

DateTime currentDateTime;

BackEase ease;
QuinticEase easeIn;

uint8_t currentAnimationMinute = 0;
int8_t minuteAnimationDirection = 1;
float minuteAnimationTimer = 0;

int previousRawBrightness = 0;
float brightness = 1;

float hourAnimationTimer = 0;

std::vector<Animation*> animations;

uint16_t OuterLED(uint16_t n)
{
    return (n + 39) % 60;
}

uint16_t InnerLED(uint16_t n)
{
    return 60 + (n + 16) % 24;
}

uint32_t colorForMinute(uint8_t minute)
{
    if (minute == 0)
        return zeroColor;
    else if (minute > currentDateTime.minute())
    {
        int currentQuarter = currentDateTime.minute() / 15;
        int targetQuarter = minute / 15;
        if ((minute % 15) == 0 && (targetQuarter - currentQuarter) == 1)
            return zeroColor;
        else
            return 0;
    }
    else if ((minute % 15) == 0)
        return quarterHourColor;
    else if ((minute % 5) == 0)
        return fiveMinuteColor;
    else
        return minuteColor;
}

uint32_t colorForHour(uint8_t hour)
{
    if (hour == 0)
        return zeroColor;
    else if (hour > currentDateTime.hour())
    {
        int currentQuarter = currentDateTime.hour() / 6;
        int targetQuarter = hour / 6;
        if ((hour % 6) == 0 && (targetQuarter - currentQuarter) == 1)
            return zeroColor;
        else
            return 0;
    }
    else if ((hour % 6) == 0)
        return quarterDayColor;
    else
        return hourColor;
}

void regenerateColors()
{
    zeroColor = strip.Color(BaseBrightness * 0.50 * brightness, BaseBrightness * 0.50 * brightness, BaseBrightness * 0.50 * brightness);
    quarterDayColor = strip.Color(0.25 * BaseBrightness * brightness, HighlightBrightness * brightness, 0.25 * BaseBrightness * brightness);
    hourColor = strip.Color(0, BaseBrightness * brightness, 0);
    hourAnimationColor = strip.Color(0, HighlightBrightness * brightness, 0);
    minuteColor = strip.Color(0, 0.625 * BaseBrightness * brightness, BaseBrightness * brightness);
    minuteAnimationColor = strip.Color(0, 0.625 * HighlightBrightness * brightness, HighlightBrightness * brightness);
    quarterHourColor = minuteAnimationColor;
    fiveMinuteColor = strip.Color(0, 0.625 * BaseBrightness * brightness * 1.5, BaseBrightness * brightness * 1.5);
    secondColor = strip.Color(BaseBrightness * brightness, 0, 0);
}

void setupLEDs()
{
    strip.begin();
    strip.show();
    
    pinMode(A0, INPUT);
    
    regenerateColors();

    Animation::neoPixels = &strip;
}

void setup()
{
    Serial.begin(9600);
    
    setupLEDs();
    
    lcd.begin(16, 2);
    lcd.print("Lohl");
    
    easeIn.setDuration(1);
    easeIn.setTotalChangeInPosition(1);
    
    Wire.begin();
    RTC.begin();
    
    RTC.enable32kHz(false);
    RTC.SQWEnable(false);
    RTC.BBSQWEnable(false);
    
    strip.setPixelColor(OuterLED(0), zeroColor);
    strip.setPixelColor(InnerLED(0), zeroColor);
    
    DateTime now = RTC.now();
    now = DateTime(now.unixtime() + TimeOffset);
    currentDateTime = now;
    for (int i = 1; i < 60; i++)
    {
        strip.setPixelColor(OuterLED(i), colorForMinute(i));
    }
    
    for (int i = 1; i < 24; i++)
    {
        strip.setPixelColor(InnerLED(i), colorForHour(i));
    }
}

void loop()
{
    DateTime now = RTC.now();
    
    now = DateTime(now.unixtime() + TimeOffset);
    
    uint32_t unixTime = now.unixtime();
    
    int rawBrightness = analogRead(A0);
    if (abs(rawBrightness - previousRawBrightness) > 2) // Compensate for jitter
    {
        brightness = ((float)rawBrightness) / 1023.0f;
        
        Serial.print("Brightness changed from: ");
        Serial.print(previousRawBrightness);
        Serial.print(" to: ");
        Serial.println(rawBrightness);
        
        regenerateColors();
        for (int i = 0; i < 60; i++)
        {
            strip.setPixelColor(OuterLED(i), colorForMinute(i));
        }
        for (int i = 0; i < 24; i++)
        {
            strip.setPixelColor(InnerLED(i), colorForHour(i));
        }
        previousRawBrightness = rawBrightness;
    }
    
    if (unixTime != previousUnixTime)
    {
        DateTime previousDateTime = currentDateTime;
        currentDateTime = now;

        if (!LEDsEnabled)
        {
            LEDStartDelayCounter++;
            if (LEDStartDelayCounter >= LEDStartDelay)
            {
                LEDsEnabled = true;
            }
        }

        if (LEDsEnabled)
        {
            if (currentDateTime.second() != 0)
            {
                FlashIn* flash = new FlashIn(OuterLED(currentDateTime.second()), 1.15, secondColor, colorForMinute(currentDateTime.second()));
                animations.push_back(flash);
            }
            
            if (currentDateTime.minute() != previousDateTime.minute())
            {
                if ((currentDateTime.minute() % 15) == 0)
                {
                    int quarter = currentDateTime.minute() / 15;
                    if (quarter != 3)
                    {
                        int minuteOfNextQuarter = (quarter + 1) * 15;
                        FadeIn *fadeIn = new FadeIn(OuterLED(minuteOfNextQuarter), 0.5, colorForMinute(minuteOfNextQuarter));
                        animations.push_back(fadeIn);
                    }
                }
                if (currentDateTime.minute() == 0)
                {
                    minuteAnimationTimer = ReverseMinuteAnimationDelay;
                    currentAnimationMinute = 59;
                    minuteAnimationDirection = -1;
                    FlashIn *flash = new FlashIn(OuterLED(currentAnimationMinute), 0.5, minuteAnimationColor, 0);
                    animations.push_back(flash);
                }
                else
                {
                    currentAnimationMinute = 1;
                    FlashIn *flash = new FlashIn(OuterLED(currentAnimationMinute), 0.5, minuteAnimationColor, colorForMinute(currentAnimationMinute));
                    animations.push_back(flash);
                    if (currentDateTime.minute() > 1)
                    {
                        minuteAnimationTimer = MinuteAnimationDelay;
                        minuteAnimationDirection = 1;
                    }
                }
            }
            
            if (currentDateTime.hour() != previousDateTime.hour())
            {
                if ((currentDateTime.hour() % 6) == 0)
                {
                    int quarter = currentDateTime.hour() / 6;
                    if (quarter != 3)
                    {
                        int hourOfNextQuarter = (quarter + 1) * 6;
                        FadeIn *fadeIn = new FadeIn(InnerLED(hourOfNextQuarter), 0.5, colorForHour(hourOfNextQuarter));
                        animations.push_back(fadeIn);
                    }
                }
                if (currentDateTime.hour() == 0)
                {
                    hourAnimationTimer = 0.25;
                }
                else
                {
                    FadeIn *fadeIn = new FadeIn(InnerLED(currentDateTime.hour()), 0.5, colorForHour(currentDateTime.hour()));
                    animations.push_back(fadeIn);
                }
            }
        }
        
        unsigned long timeSinceLastTick = millis() - lastSecondMillis;
        
        char time[9];
        snprintf(time, 9, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
        
//        char date[11];
//        snprintf(date, 11, "%02d.%02d.%04d", now.day(), now.month(), now.year());
        
        char brightnessString[5];
        brightnessString[3] = ' ';
        brightnessString[4] = '\0';
        
        snprintf(brightnessString, 5, "%d%%", ((int)(brightness * 100.0f)));
        
        lcd.setCursor(0,0);
        lcd.print(time);
        lcd.setCursor(0,1);
        lcd.print(brightnessString);
        lcd.print("   ");
        
        char ms[6];
        snprintf(ms, 6, "%dms", timeSinceLastTick);
        
        char fps[6];
        snprintf(fps, 6, "%dfps", updatesPerSecond);
        
        lcd.setCursor(11,0);
        lcd.print(ms);
        lcd.setCursor(11,1);
        lcd.print(fps);
        
        lastSecondMillis = millis();
        previousUnixTime = unixTime;
        millisPerSecond = timeSinceLastTick;
        updatesPerSecond = 0;
    }
    else
    {
        updateSkipCounter++;
        if (updateSkipCounter == UpdateSkips)
        {
            unsigned long currentMillis = millis();
            
            double timeMultiplier = 1000.0 / ((double)millisPerSecond);
            
            unsigned long timeSinceLastUpdate = currentMillis - lastUpdateMillis;
            float time = timeSinceLastUpdate * timeMultiplier / 1000.0;
            
            if (hourAnimationTimer > 0)
            {
                hourAnimationTimer -= time;
                float amount = hourAnimationTimer / 0.25;
                amount = constrain(amount, 0, 1);
                float scaledAmount = easeIn.easeOut(amount);
                for (int i = 1; i < 24; i++)
                {
                    strip.setPixelColor(InnerLED(i), 0, 30 * scaledAmount, 0);
                }
                if (hourAnimationTimer <= 0)
                    hourAnimationTimer = 0;
            }
            
            if (minuteAnimationTimer > 0)
            {
                minuteAnimationTimer -= time;
                if (minuteAnimationTimer <= 0)
                {
                    currentAnimationMinute += minuteAnimationDirection;
                    
                    Animation *flash;
                    
                    if (currentAnimationMinute == currentDateTime.minute())
                    {
                        flash = new FlashIn(OuterLED(currentAnimationMinute), 0.5, minuteAnimationColor, colorForMinute(currentAnimationMinute));
                    }
                    else
                    {
                        if (minuteAnimationDirection > 0)
                            flash = new FlashIn(OuterLED(currentAnimationMinute), 0.5, minuteAnimationColor, colorForMinute(currentAnimationMinute));
                        else
                            flash = new FlashIn(OuterLED(currentAnimationMinute), 0.5, minuteAnimationColor, 0);
                    }
                    animations.push_back(flash);
                    
                    if (minuteAnimationDirection > 0 && currentAnimationMinute < currentDateTime.minute())
                    {
                        minuteAnimationTimer = MinuteAnimationDelay;
                    }
                    else if (minuteAnimationDirection < 0 && currentAnimationMinute > 0)
                    {
                        minuteAnimationTimer = ReverseMinuteAnimationDelay;
                    }
                }
            }
            
            if (LEDsEnabled)
            {
                for(std::vector<Animation*>::iterator it = animations.begin(); it != animations.end(); )
                {
                    Animation* animation = *it;
                    if (!animation->update(time))
                    {
                        it = animations.erase(it);
                        delete animation;
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
            strip.show();
            updatesPerSecond++;
            updateSkipCounter = 0;
            lastUpdateMillis = currentMillis;
        }
    }
}