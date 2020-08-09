#ifndef EFFECTS_H
#define EFFECTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../commands/commands.h"

const uint32_t Wheel(Adafruit_NeoPixel *strip, byte WheelPos);
const void rainbow(Adafruit_NeoPixel *strip, uint8_t wait);
const void doubleRainbow(Adafruit_NeoPixel *strip, uint8_t wait);
void meteorRain(Adafruit_NeoPixel *strip, byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void showStrip(Adafruit_NeoPixel *strip);
void fadeToBlack(Adafruit_NeoPixel *strip, int ledNo, byte fadeValue);

#endif