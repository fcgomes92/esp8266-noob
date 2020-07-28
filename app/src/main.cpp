#include <Arduino.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include "effects/effects.h"
#include "utils/utils.h"
#include "commands/commands.h"

// LED Strip definition
#define NAME "neopixel"
#define PIN D2
#define PIXELS 144
#define DEBUG true //if enabled this increases the latency
#define LOG(...) \
    if (DEBUG)   \
    Serial.println(__VA_ARGS__)
Adafruit_NeoPixel *strip;

int selectedEffect = 1;
boolean isEffectActive = true;

// WebSocket Client definition
boolean handshakeFailed = 0;
String data = "";
char connectionPath[] = "/";
char *ssid = (char *)"BatataFrita";
char *password = (char *)"batataassadaS2";
const int espport = 3000;
char *host = (char *)"manhattan.local";
WebSocketClient webSocketClient;
WiFiClient client;

// Handle data definition
const size_t jsonReceiveDataCapacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;
long connectPreviousMillis = 0;
unsigned long connectInterval = 150;
unsigned long dataPreviousMillis = 0;
unsigned long datatInterval = 100;

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
            // getStripState(webSocketClient, strip, isEffectActive, selectedEffect);
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
                        commandFill(webSocketClient, strip, doc);
                        break;
                    case 1:
                        setBrightness(webSocketClient, strip, doc);
                        break;
                    case 2:
                        strip->clear();
                        break;
                    case 3:
                        toggleEffect(webSocketClient, strip, doc, &isEffectActive, &selectedEffect);
                        break;
                    case 4:
                        setPixels(webSocketClient, strip, doc);
                        break;
                    case 5:
                        getStripState(webSocketClient, strip, isEffectActive, selectedEffect);
                        break;
                    }
                    strip->show();
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

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    strip = new Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);
    strip->begin();
    strip->setBrightness(255);
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

void loop()
{
    handleData();
    if (isEffectActive)
    {
        switch (selectedEffect)
        {
        case 1:
            rainbow(strip, 25);
            break;
        case 2:
            CylonBounce(strip, 0xff, 0, 0, 4, 10, 50);
            break;
        case 3:
            meteorRain(strip, 0xff, 0xff, 0xff, 10, 64, true, 30);
            break;
        }
    }
}