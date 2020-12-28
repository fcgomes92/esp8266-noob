#include "utils.h"

const bool validRGB(int r, int g, int b)
{
    return (r >= 0 && r <= 255) && (g >= 0 && g <= 255) && (b >= 0 && b <= 255);
}

const bool validBrightness(int v)
{
    return v >= 0 && v <= 255;
}

const bool validEffect(int v)
{
    return v >= 0 && v <= 55;
}