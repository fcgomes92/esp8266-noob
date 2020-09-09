#include "breath.h"

void startBreath(Adafruit_NeoPixel *strip, BreathConfig *config)
{
    config->startTime = millis();
    strip->fill(strip->Color(config->colorR, config->colorG, config->colorB));
    strip->show();
}

void updateBreath(Adafruit_NeoPixel *strip, BreathConfig *config)
{
    const float pi = 3.14;
    float frac; // ratio of color to use, based on time
    int r, g, b;

    // calculate a brightness fraction, based on a sine curve changing over time from 0 to 1 and back
    frac = (sin((((millis() - config->startTime) % config->breathTime) / config->breathTimeFloat - 0.25f) * 2.0f * pi) + 1.0f) / 2.0f;

    // multiply each color by the brightness fraction
    r = config->colorR * frac;
    g = config->colorG * frac;
    b = config->colorB * frac;

    strip->fill(strip->Color(r, g, b));
    strip->show();
}

void createBreathConfig(BreathConfig *config, unsigned long startTime, int r, int g, int b, int breathTime)
{
    config->startTime = startTime;
    config->colorR = r;
    config->colorG = g;
    config->colorB = g;
    config->breathTime = breathTime;
    config->breathTimeFloat = (float)breathTime;
}