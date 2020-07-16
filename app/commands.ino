#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <WebSocketClient.h>

void getStripState()
{
    String output;
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(144) + JSON_OBJECT_SIZE(1));
    if (isEffectActive)
    {
        doc["effect"] = selectedEffect;
    }
    else
    {
        doc["effect"] = -1;
        for (int i = 0; i < PIXELS; i++)
        {
            doc["pixels"][i] = strip.getPixelColor(i);
        }
    }
    serializeJson(doc, output);
    //    serializeJson(doc, Serial);
    webSocketClient.sendData(output);
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue)
{
    for (int i = 0; i < strip.numPixels(); i++)
    {
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void commandFill(DynamicJsonDocument doc)
{
    int r = doc["r"].as<int>();
    int g = doc["g"].as<int>();
    int b = doc["b"].as<int>();
    if (validRGB(r, g, b))
    {
        uint32_t color = strip.Color(r, g, b);
        strip.fill(color);
    }
}

void setBrightness(DynamicJsonDocument doc)
{
    int brightness = doc["v"].as<int>();
    if (validBrightness(brightness))
    {
        strip.setBrightness(brightness);
    }
}

void toggleEffect(DynamicJsonDocument doc)
{
    selectedEffect = doc["v"].as<int>();
    isEffectActive = doc["s"].as<bool>();
    Serial.println("Set effect to: " + String(isEffectActive));
    Serial.println("Selected effect: " + String(selectedEffect));
}

void setPixels(DynamicJsonDocument doc)
{
    int r = doc["r"].as<int>();
    int g = doc["g"].as<int>();
    int b = doc["b"].as<int>();
    if (!validRGB(r, g, b))
    {
        Serial.println("Error creating color: " + String(r) + ", " + String(g) + ", " + String(b));
        return;
    }
    uint32_t color = strip.Color(r, g, b);
    // start pixel
    int s = doc["s"].as<int>();
    // end pixel
    int e = doc["e"].as<int>();
    // from start (s) pixel to end (e) pixel
    for (int i = s; i < e; i++)
    {
        strip.setPixelColor(i, color);
    }
}