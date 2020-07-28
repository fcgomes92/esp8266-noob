#ifndef COMMANDS_H
#define COMMANDS_H

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <WebSocketClient.h>
#include "../utils/utils.h"
#include "../effects/effects.h"

void getStripState(WebSocketClient webSocketClient, Adafruit_NeoPixel *strip, bool isEffectActive, int selectedEffect);
void setPixel(Adafruit_NeoPixel *strip, int pixel, byte red, byte green, byte blue);
void setAll(Adafruit_NeoPixel *strip, byte red, byte green, byte blue);
void commandFill(WebSocketClient webSocketClient, Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
void setBrightness(WebSocketClient webSocketClient, Adafruit_NeoPixel *strip, DynamicJsonDocument doc);
void toggleEffect(WebSocketClient webSocketClient, Adafruit_NeoPixel *strip, DynamicJsonDocument doc, bool *isEffectActive, int *selectedEffect);
void setPixels(WebSocketClient webSocketClient, Adafruit_NeoPixel *strip, DynamicJsonDocument doc);

#endif