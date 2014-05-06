#include "Arduino.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>

#define UpdateSkips 8
#define LEDStartDelay 3

RTC_DS3231 RTC;
LiquidCrystal lcd(11, 10, 9, 12, 4, 3);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60 + 24, 6, NEO_GRB + NEO_KHZ800);

uint8_t updateSkipCounter = 0;
uint8_t LEDStartDelayCounter = 0;
boolean LEDsEnabled = false;

unsigned long millisPerSecond = 0;
uint16_t updatesPerSecond = 0;

unsigned long lastSecondMillis = 0;
uint32_t previousUnixTime = 0;

DateTime currentDateTime;

BackEase ease;

uint16_t OuterLED(uint16_t n)
{
    return (n + 39) % 60;
}

uint16_t InnerLED(uint16_t n)
{
    return 60 + (n + 16) % 24;
}

void setupLEDs()
{
    strip.begin();
    strip.show();
}

void setup()
{
    setupLEDs();
    
    ease.setDuration(.5);
    ease.setTotalChangeInPosition(1);
    
    lcd.begin(16, 2);
    lcd.print("Lohl");
    
    Wire.begin();
    RTC.begin();
    
    RTC.enable32kHz(false);
    RTC.SQWEnable(false);
    RTC.BBSQWEnable(false);
}

void loop()
{
    DateTime now = RTC.now();
    
    uint32_t unixTime = now.unixtime();
    
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
        
        if (currentDateTime.hour() != previousDateTime.hour())
        {
            for (int i = 0; i < currentDateTime.hour(); i++)
            {
                strip.setPixelColor(InnerLED(i), 0, 15, 0);
            }
        }
        
        if (currentDateTime.minute() != previousDateTime.minute())
        {
            for (int i = 0; i < currentDateTime.minute(); i++)
            {
                strip.setPixelColor(OuterLED(i), 0, 0, 15);
            }
        }
        
        if (LEDsEnabled)
            strip.setPixelColor(OuterLED(previousDateTime.second()), 0, 0, 0);
        
        unsigned long timeSinceLastTick = millis() - lastSecondMillis;
        
        char time[9];
        snprintf(time, 9, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
        
        char date[11];
        snprintf(date, 11, "%02d.%02d.%04d", now.day(), now.month(), now.year());
        
        lcd.setCursor(0,0);
        lcd.print(time);
        lcd.setCursor(0,1);
        lcd.print(date);
        
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
            unsigned long timeSinceLastTick = millis() - lastSecondMillis;
            double amount = timeSinceLastTick / ((double)millisPerSecond);
            uint8_t currentSecond = currentDateTime.second();
            uint8_t nextSecond = (currentSecond == 59) ? 0 : currentSecond + 1;
            if (LEDsEnabled)
            {
                uint8_t brightness = 30;
                
                double scaledAmount;
                
                if (amount <= 0.5)
                    scaledAmount = ease.easeOut(amount);
                else
                    scaledAmount = 1.0 - ease.easeIn(amount - 0.5);
                
                if (currentSecond <= currentDateTime.minute())
                    strip.setPixelColor(OuterLED(currentSecond), brightness * scaledAmount, 0, 0);
                else
                    strip.setPixelColor(OuterLED(currentSecond), brightness * scaledAmount, 0, 0);

//                if (nextSecond <= currentDateTime.minute())
//                    strip.setPixelColor(OuterLED(nextSecond), brightness * amount, 0, 15 * (1.0 - amount));
//                else
//                    strip.setPixelColor(OuterLED(nextSecond), brightness * amount, 0, 0);
            }
            strip.show();
            updatesPerSecond++;
            updateSkipCounter = 0;
        }
    }
}