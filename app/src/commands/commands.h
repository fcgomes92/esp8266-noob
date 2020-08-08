#ifndef COMMANDS_H
#define COMMANDS_H

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <string>
#include "../utils/utils.h"
#include "../effects/effects.h"

DynamicJsonDocument getStripState(Adafruit_NeoPixel *strip, bool isEffectActive, int selectedEffect);
DynamicJsonDocument setPixel(Adafruit_NeoPixel *strip, int pixel, byte red, byte green, byte blue);
DynamicJsonDocument setAll(Adafruit_NeoPixel *strip, byte red, byte green, byte blue);
DynamicJsonDocument commandFill(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
DynamicJsonDocument setBrightness(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
DynamicJsonDocument toggleEffect(Adafruit_NeoPixel *strip, DynamicJsonDocument doc, bool *isEffectActive, int *selectedEffect);
DynamicJsonDocument setPixels(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);

#endif