#ifdef __AVR__
#include <avr/power.h>
#endif
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>

// LED Strip definition
#define NAME "neopixel"
#define PIN D2
#define PIXELS 144
#define DEBUG false //if enabled this increases the latency
#define LOG(...) \
  if (DEBUG)     \
  Serial.println(__VA_ARGS__)
Adafruit_NeoPixel strip(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int isEffectActive = 1;
int selectedEffect = 1;

// WebSocket Client definition
boolean handshakeFailed = 0;
String data = "";
char connectionPath[] = "/";
char *ssid = "BatataFrita";
char *password = "batataassadaS2";
const int espport = 3000;
char *host = "manhattan.local";
WebSocketClient webSocketClient;
WiFiClient client;

// Handle data definition
const size_t jsonReceiveDataCapacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;
long connectPreviousMillis = 0;
long connectInterval = 150;
long dataPreviousMillis = 0;
long datatInterval = 100;

void wsConnect()
{
  // Connect to the websocket server
  if (client.connect(host, espport))
  {
    LOG("Connected");
    webSocketClient.host = host;
    webSocketClient.path = connectionPath;
    if (webSocketClient.handshake(client))
    {
      LOG("Handshake successful");
      getStripState();
    }
    else
    {
      LOG("Handshake failed.");
    }
  }
  else
  {
    LOG("Connection failed.");
  }
}

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  strip.begin();
  strip.setBrightness(255);
  Serial.begin(115200);

  delay(10);

  LOG();
  LOG();
  Serial.print("Connecting to ");
  LOG(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  LOG("");
  LOG("WiFi connected");
  LOG("IP address: ");
  LOG(WiFi.localIP());
  delay(750);
  wsConnect();
}

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
        DynamicJsonDocument doc(jsonReceiveDataCapacity);
        LOG(data);
        DeserializationError error = deserializeJson(doc, data);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          LOG(error.c_str());
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
      wsConnect();
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
