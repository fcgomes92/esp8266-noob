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
    int r = doc["r"].as<int>();
    int g = doc["g"].as<int>();
    int b = doc["b"].as<int>();
    if (validRGB(r, g, b))
    {
        uint32_t color = strip->Color(r, g, b);
        strip->fill(color);
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