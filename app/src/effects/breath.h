#ifndef BREATH_H
#define BREATH_H

#include <stdio.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

struct BreathConfig
{
    unsigned long startTime = 0;
    int colorR = 150;
    int colorG = 0;
    int colorB = 200;
    int breathTime = 9000; // 6000ms = six seconds; adjust for your desired speed
    float breathTimeFloat = 9000.0;
    float pi = 3.14;
    BreathConfig() {}
    BreathConfig(unsigned long startTime, int r, int g, int b, int breathTime)
        : startTime(startTime), colorR(r), colorG(g), colorB(b), breathTime(breathTime), breathTimeFloat(breathTime), pi(3.14)
    {
    }
};
void startBreath(Adafruit_NeoPixel *strip, BreathConfig *config);
void startBreath(Adafruit_NeoPixel *strip, BreathConfig *config);
void updateBreath(Adafruit_NeoPixel *strip, BreathConfig *config);
void createBreathConfig(BreathConfig *config, unsigned long startTime, int r, int g, int b, int breathTime);

#endif