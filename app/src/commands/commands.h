#ifndef COMMANDS_H
#define COMMANDS_H

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <string>
#include "../utils/utils.h"
#include "../effects/effects.h"

String getStripState(Adafruit_NeoPixel *strip, bool isEffectActive, int selectedEffect);
String setPixel(Adafruit_NeoPixel *strip, int pixel, byte red, byte green, byte blue);
String setAll(Adafruit_NeoPixel *strip, byte red, byte green, byte blue);
String commandFill(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
String setBrightness(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
String toggleEffect(Adafruit_NeoPixel *strip, DynamicJsonDocument doc, bool *isEffectActive, int *selectedEffect);
String setPixels(Adafruit_NeoPixel *strip, DynamicJsonDocument doc);

#endif