#include "commands.h"

DynamicJsonDocument getStripState(WS2812FX *strip, DynamicJsonDocument doc)
{
    doc["effect"] = strip->getMode();
    doc["color"] = strip->getColor();
    doc["speed"] = strip->getSpeed();
    return doc;
}

DynamicJsonDocument commandFill(WS2812FX *strip, DynamicJsonDocument doc)
{
    uint8_t r = doc["r"].as<uint8_t>();
    uint8_t g = doc["g"].as<uint8_t>();
    uint8_t b = doc["b"].as<uint8_t>();
    strip->setMode(0);
    if (validRGB(r, g, b))
    {
        uint32_t startColor = strip->getColor();
        uint8_t currentR = startColor >> 16, currentG = startColor >> 8, currentB = startColor;
        uint8_t modR = currentR > r ? -1 : 1, modG = currentG > g ? -1 : 1, modB = currentB > b ? -1 : 1;
        while (currentR != r || currentG != g || currentB != b)
        {
            if (currentR != r)
                currentR = currentR + (1 * modR);
            if (currentG != g)
                currentG = currentG + (1 * modG);
            if (currentB != b)
                currentB = currentB + (1 * modB);
            strip->setColor(strip->Color(currentR, currentG, currentB));
            delay(10);
        }
        strip->setColor(strip->Color(r, g, b));
    }
    return doc;
}

DynamicJsonDocument setStripEffect(WS2812FX *strip, DynamicJsonDocument doc)
{
    int effect = doc["e"].as<int>();
    if (!validEffect(effect))
    {
        doc["error"] = "true";
        doc["message"] = "Error setting effect: " + String(effect);
        return doc;
    }
    else
    {
        strip->setMode(effect);
        if (doc["r"] && doc["g"] && doc["b"])
        {
            setStripColor(strip, doc);
        }
        return doc;
    }
}

DynamicJsonDocument setBrightness(WS2812FX *strip, DynamicJsonDocument doc)
{
    int brightness = doc["v"].as<int>();
    if (validBrightness(brightness))
    {
        strip->setBrightness(brightness);
    }
    return doc;
}

DynamicJsonDocument setStripColor(WS2812FX *strip, DynamicJsonDocument doc)
{
    int r = doc["r"].as<int>();
    int g = doc["g"].as<int>();
    int b = doc["b"].as<int>();
    if (!validRGB(r, g, b))
    {
        doc["error"] = "true";
        doc["message"] = "Error creating color: " + String(r) + ", " + String(g) + ", " + String(b);
        return doc;
    }
    uint32_t color = strip->Color(r, g, b);
    strip->setColor(color);
    return doc;
}

DynamicJsonDocument setStripSpeed(WS2812FX *strip, DynamicJsonDocument doc)
{
    int v = doc["v"].as<int>();
    strip->setSpeed(v);
    return doc;
}