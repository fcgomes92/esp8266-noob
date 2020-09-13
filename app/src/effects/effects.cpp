#include "effects.h"

/** Effects **/
const uint32_t Wheel(Adafruit_NeoPixel *strip, byte WheelPos)
{
    if (WheelPos < 85)
    {
        return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

const void updateRainbowCycle(Adafruit_NeoPixel *strip, RainbowConfig *config)
{
    if ((millis() - config->lastUpdate) > config->interval)
    {
        config->lastUpdate = millis();
        uint16_t j = config->colorIndex;
        uint16_t i = 0;
        for (i = 0; i < strip->numPixels(); i++)
        {
            strip->setPixelColor(i, Wheel(strip, ((i * 256 / strip->numPixels()) + j) & 255));
        }
        strip->show();
        config->colorIndex = config->colorIndex < 255 ? config->colorIndex + 1 : 0;
    }
}

const void updateRainbow(Adafruit_NeoPixel *strip, RainbowConfig *config)
{
    if ((millis() - config->lastUpdate) > config->interval)
    {
        config->lastUpdate = millis();
        uint16_t j = config->colorIndex;
        uint16_t i = 0;
        for (i = 0; i < strip->numPixels(); i++)
        {
            strip->setPixelColor(i, Wheel(strip, ((i + j) & 255)));
        }
        strip->show();
        config->colorIndex = config->colorIndex < 255 ? config->colorIndex + 1 : 0;
    }
}

const void updateDoubleRainbow(Adafruit_NeoPixel *strip, RainbowConfig *config)
{
    if ((millis() - config->lastUpdate) > config->interval)
    {
        config->lastUpdate = millis();
        uint16_t j = config->colorIndex;
        uint16_t i = 0;
        int size = strip->numPixels() / 2 - 1;
        for (i = 0; i < size; i++)
        {
            strip->setPixelColor(i, Wheel(strip, (i + j) & 255));
            strip->setPixelColor(size * 2 - i, Wheel(strip, (i + j) & 255));
        }
        strip->show();
        config->colorIndex = config->colorIndex < 255 ? config->colorIndex + 1 : 0;
    }
}

void meteorRain(Adafruit_NeoPixel *strip, byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    setAll(strip, 0, 0, 0);

    for (int i = 0; i < strip->numPixels() + strip->numPixels(); i++)
    {

        // fade brightness all LEDs one step
        for (int j = 0; j < strip->numPixels(); j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                fadeToBlack(strip, j, meteorTrailDecay);
            }
        }

        // draw meteor
        for (int j = 0; j < meteorSize; j++)
        {
            if ((i - j < strip->numPixels()) && (i - j >= 0))
            {
                setPixel(strip, i - j, red, green, blue);
            }
        }

        showStrip(strip);
        delay(SpeedDelay);
    }
}

void showStrip(Adafruit_NeoPixel *strip)
{
    strip->show();
}

void fadeToBlack(Adafruit_NeoPixel *strip, int ledNo, byte fadeValue)
{
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;

    oldColor = strip->getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

    strip->setPixelColor(ledNo, r, g, b);
}