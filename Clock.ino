#include "Arduino.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "Flash.h"
#include "StandardCplusplus.h"
#include <vector>

#define Brightness 30
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

unsigned long lastUpdateMillis = 0;
unsigned long lastSecondMillis = 0;
uint32_t previousUnixTime = 0;

DateTime currentDateTime;

BackEase ease;

std::vector<Flash*> flashes;

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
    
    Flash::neoPixels = &strip;
}

void setup()
{
    Serial.begin(9600);
    
    setupLEDs();
    
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

        if (LEDsEnabled)
        {
            Flash* flash = new Flash(OuterLED(currentDateTime.second()), 1.15, Brightness, 0, 0);
            flashes.push_back(flash);
        }
        
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
            unsigned long currentMillis = millis();
            
            double timeMultiplier = 1000.0 / ((double)millisPerSecond);
            
            unsigned long timeSinceLastUpdate = currentMillis - lastUpdateMillis;
            float time = timeSinceLastUpdate * timeMultiplier / 1000.0;

            if (LEDsEnabled)
            {
                for(std::vector<Flash*>::iterator it = flashes.begin(); it != flashes.end(); )
                {
                    Flash* flash = *it;
                    if (!flash->update(time))
                    {
                        it = flashes.erase(it);
                        delete flash;
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