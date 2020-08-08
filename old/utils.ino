const bool validRGB(int r, int g, int b)
{
    if (r < 0 || r > 255)
        return false;
    if (g < 0 || g > 255)
        return false;
    if (b < 0 || b > 255)
        return false;
    return true;
}

const bool validBrightness(int v)
{
    if (v < 0 || v > 255)
        return false;
    return true;
}
