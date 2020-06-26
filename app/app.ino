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
const int espport = 3000;\
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
  uint16_t i, j;

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
  strip.setBrightness(80); // 1/3 brightness
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

bool validRGB(int r, int g, int b) {
  if (r < 0 || r > 254) return false;
  if (g < 0 || g > 254) return false;
  if (b < 0 || b > 254) return false;
  return true;
}


bool validBrightness(int v) {
  if (v < 0 || v > 254) return false;
  return true;
}

void commandFill(DynamicJsonDocument doc)
{
  int r = doc["params"]["r"].as<int>();
  int g = doc["params"]["g"].as<int>();
  int b = doc["params"]["b"].as<int>();
  if (validRGB(r,g,b)) {
    uint32_t color = strip.Color(r, g, b);
    strip.fill(color);
  }
}

void setBrightness(DynamicJsonDocument doc) {
  int brightness = doc["params"]["v"].as<int>();
  if (validBrightness(brightness)) {
    strip.setBrightness(brightness);
  }
}

void selectEffect(DynamicJsonDocument doc) {
  selectedEffect = doc["params"]["v"].as<int>();
}

void loop()
{
  if (client.connected())
  {
    currentMillis = millis();
    webSocketClient.getData(data);
    if (data.length() > 0)
    {
      const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
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
        long command = doc["command"];
        Serial.println(command);
        switch (command)
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
          isEffectActive = !isEffectActive;
          break;
        case 4:
          selectEffect(doc);
          break;
        }
        strip.show();
      }
      data = "";
      delay(5);
    }
    else
    {
//      Serial.println("Sending default data:");
//      webSocketClient.sendData("da-data!");
    }
  }
  else
  {
    delay(15);
    wsconnect();
  }
  if (isEffectActive) {
      switch(selectedEffect) {
        case 1:
          rainbow(25);
      }
    }
}
//*********************************************************************************************************************
//***************function definitions**********************************************************************************
void wsconnect()
{
  // Connect to the websocket server
  if (client.connect(host, espport))
  {
    Serial.println("Connected");
  }
  else
  {
    Serial.println("Connection failed.");
    delay(1000);

    if (handshakeFailed)
    {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client))
  {
    Serial.println("Handshake successful");
  }
  else
  {

    Serial.println("Handshake failed.");
    delay(4000);

    if (handshakeFailed)
    {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
}
