#ifndef EFFECTS_H
#define EFFECTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../commands/commands.h"

struct RainbowConfig
{
    /**
     * Types:
     * - 0: rainbowCycle
     * - 1: rainbow
     * - 2: doubleRainbow
     * */
    int type = 0;
    unsigned long interval = 9000; // 6000ms = six seconds; adjust for your desired speed
    unsigned long lastUpdate = 0;  // 6000ms = six seconds; adjust for your desired speed
    uint16_t colorIndex = 0;
    RainbowConfig() {}
    RainbowConfig(unsigned long interval)
        : interval(interval), colorIndex(0), lastUpdate(0)
    {
    }
};

const uint32_t Wheel(Adafruit_NeoPixel *strip, byte WheelPos);
const void updateRainbowCycle(Adafruit_NeoPixel *strip, RainbowConfig *config);
const void updateRainbow(Adafruit_NeoPixel *strip, RainbowConfig *config);
const void updateDoubleRainbow(Adafruit_NeoPixel *strip, RainbowConfig *config);
void meteorRain(Adafruit_NeoPixel *strip, byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void showStrip(Adafruit_NeoPixel *strip);
void fadeToBlack(Adafruit_NeoPixel *strip, int ledNo, byte fadeValue);

#endif