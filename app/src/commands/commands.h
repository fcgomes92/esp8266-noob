#ifndef COMMANDS_H
#define COMMANDS_H

#include <ArduinoJson.h>
#include <WS2812FX.h>
#include <PubSubClient.h>
#include <string>
#include "../utils/utils.h"

DynamicJsonDocument getStripState(WS2812FX *strip, DynamicJsonDocument doc);
DynamicJsonDocument setAll(WS2812FX *strip, byte red, byte green, byte blue);
DynamicJsonDocument commandFill(WS2812FX *strip, DynamicJsonDocument doc);
DynamicJsonDocument setBrightness(WS2812FX *strip, DynamicJsonDocument doc);
DynamicJsonDocument setStripEffect(WS2812FX *strip, DynamicJsonDocument doc);
DynamicJsonDocument setStripColor(WS2812FX *strip, DynamicJsonDocument doc);
DynamicJsonDocument setStripSpeed(WS2812FX *strip, DynamicJsonDocument doc);

#endif