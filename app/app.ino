#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define NAME "neopixel"
#define PIN D2
#define PIXELS 144
#define DEBUG true //if enabled this increases the latency
Adafruit_NeoPixel strip(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define CMD_APPLY 0x01
#define CMD_SET 0x02
#define CMD_FILL 0x03
#define CMD_OFF 0x04

boolean handshakeFailed = 0;
String data = "";
char path[] = "/"; //identifier of this device*-
const char *ssid = "BatataFrita";
const char *password = "batataassadaS2";
char *host = "192.168.0.110"; //replace this ip address with the ip address of your Node.Js server
const int espport = 3000;
int isEffectActive = 1;
int selectedEffect = 1;

WebSocketClient webSocketClient;
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval = 300; //interval for sending data to the websocket server in ms
// Use WiFiClient class to create TCP connections
WiFiClient client;

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait)
{
  uint16_t i = 0;
  uint16_t j = 0;
  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i * 1 + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pinMode(D1, OUTPUT); //Declare Pin mode
  strip.begin();
  strip.setBrightness(255); // 1/3 brightness
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  wsconnect();
  //  wifi_set_sleep_type(LIGHT_SLEEP_T);
}

bool validRGB(int r, int g, int b)
{
  if (r < 0 || r > 255)
    return false;
  if (g < 0 || g > 255)
    return false;
  if (b < 0 || b > 255)
    return false;
  return true;
}

bool validBrightness(int v)
{
  if (v < 0 || v > 255)
    return false;
  return true;
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

//const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
const size_t capacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;
long connectPreviousMillis = 0;
long connectInterval = 15;
long dataPreviousMillis = 0;
long datatInterval = 5;

void handleData()
{
  unsigned long currentMillis = millis();
  if (client.connected())
  {
    if (currentMillis - dataPreviousMillis > datatInterval)
    {
      currentMillis = millis();
      webSocketClient.getData(data);
      if (data.length() > 0)
      {
        DynamicJsonDocument doc(capacity);
        Serial.println(data);
        DeserializationError error = deserializeJson(doc, data);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
        }
        else
        {
          switch (doc["c"].as<int>())
          {
          case 0:
            commandFill(doc);
            break;
          case 1:
            setBrightness(doc);
            break;
          case 2:
            strip.clear();
            break;
          case 3:
            toggleEffect(doc);
            break;
          case 4:
            setPixels(doc);
            break;
          case 5:
            getStripState();
            break;
          }
          strip.show();
        }
        data = "";
      }
      dataPreviousMillis = currentMillis;
    }
  }
  else
  {
    if (currentMillis - connectPreviousMillis > connectInterval)
    {
      wsconnect();
      connectPreviousMillis = currentMillis;
    }
  }
}

void loop()
{
  handleData();
  if (isEffectActive)
  {
    switch (selectedEffect)
    {
    case 1:
      rainbow(25);
      break;
    case 2:
      CylonBounce(0xff, 0, 0, 4, 10, 50);
      break;
    case 3:
      meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
      break;
    }
  }
}
//*********************************************************************************************************************
//***************function definitions**********************************************************************************
void getStripState()
{
  String output;
  if (isEffectActive)
  {
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
    doc["effect"] = selectedEffect;
    serializeJson(doc, output);
//    serializeJson(doc, Serial);
    webSocketClient.sendData(output);
  }
  else
  {
    DynamicJsonDocument doc(JSON_ARRAY_SIZE(144) + JSON_OBJECT_SIZE(1));
    for (int i = 0; i < PIXELS; i++)
    {
      doc["pixels"][i] = strip.getPixelColor(i);
    }
    serializeJson(doc, output);
    serializeJson(doc, Serial);
    webSocketClient.sendData(output);
  }
}

void wsconnect()
{
  // Connect to the websocket server
  if (client.connect(host, espport))
  {
    Serial.println("Connected");
    // Handshake with the server
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client))
    {
      Serial.println("Handshake successful");
      getStripState();
    }
    else
    {

      Serial.println("Handshake failed.");
      //    if (handshakeFailed)
      //    {
      //      handshakeFailed = 0;
      //      ESP.restart();
      //    }
      //    handshakeFailed = 1;
    }
  }
  else
  {
    Serial.println("Connection failed.");
    //    delay(1000);

    //    if (handshakeFailed)
    //    {
    //      handshakeFailed = 0;
    //      ESP.restart();
    //    }
    //    handshakeFailed = 1;
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
  setAll(0, 0, 0);

  for (int i = 0; i < strip.numPixels() + strip.numPixels(); i++)
  {

    // fade brightness all LEDs one step
    for (int j = 0; j < strip.numPixels(); j++)
    {
      if ((!meteorRandomDecay) || (random(10) > 5))
      {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++)
    {
      if ((i - j < strip.numPixels()) && (i - j >= 0))
      {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{

  for (int i = 0; i < strip.numPixels() - EyeSize - 2; i++)
  {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++)
    {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = strip.numPixels() - EyeSize - 2; i > 0; i--)
  {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++)
    {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void showStrip()
{
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
