#include "commands.h"
#include "../effects/breath.h"

DynamicJsonDocument getStripState(Adafruit_NeoPixel *strip, bool isEffectActive, int selectedEffect)
{
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(144) + JSON_OBJECT_SIZE(1));
    if (isEffectActive)
    {
        doc["effect"] = selectedEffect;
    }
    else
    {
        doc["effect"] = -1;
        for (int i = 0; i < strip->numPixels(); i++)
        {
            doc["pixels"][i] = strip->getPixelColor(i);
        }
    }
    //    serializeJson(doc, Serial);
    return doc;
}

DynamicJsonDocument setPixel(Adafruit_NeoPixel *strip, int pixel, byte red, byte green, byte blue)
{
    strip->setPixelColor(pixel, strip->Color(red, green, blue));
    return DynamicJsonDocument(JSON_OBJECT_SIZE(1));
}

DynamicJsonDocument setAll(Adafruit_NeoPixel *strip, byte red, byte green, byte blue)
{
    for (int i = 0; i < strip->numPixels(); i++)
    {
        setPixel(strip, i, red, green, blue);
    }
    showStrip(strip);
    return DynamicJsonDocument(JSON_OBJECT_SIZE(1));
}

DynamicJsonDocument commandFill(Adafruit_NeoPixel *strip, DynamicJsonDocument doc)
{
    uint8_t r = doc["r"].as<uint8_t>();
    uint8_t g = doc["g"].as<uint8_t>();
    uint8_t b = doc["b"].as<uint8_t>();
    if (validRGB(r, g, b))
    {
        uint32_t startColor = strip->getPixelColor(0);
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
            strip->fill(strip->Color(currentR, currentG, currentB));
            strip->show();
            delay(10);
        }
        strip->fill(strip->Color(r, g, b));
    }
    return doc;
}

DynamicJsonDocument setBrightness(Adafruit_NeoPixel *strip, DynamicJsonDocument doc)
{
    int brightness = doc["v"].as<int>();
    if (validBrightness(brightness))
    {
        strip->setBrightness(brightness);
    }
    return doc;
}

DynamicJsonDocument toggleEffect(Adafruit_NeoPixel *strip, DynamicJsonDocument doc, bool *isEffectActive, int *selectedEffect)
{
    *selectedEffect = doc["v"].as<int>();
    *isEffectActive = doc["s"].as<bool>();
    return doc;
}
DynamicJsonDocument setPixels(Adafruit_NeoPixel *strip, DynamicJsonDocument doc)
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
    // start pixel
    int s = doc["s"].as<int>();
    // end pixel
    int e = doc["e"].as<int>();
    // from start (s) pixel to end (e) pixel
    for (int i = s; i < e; i++)
    {
        strip->setPixelColor(i, color);
        strip->show();
    }
    return doc;
}