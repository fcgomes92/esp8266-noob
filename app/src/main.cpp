#include <stdio.h>
#include <EEPROM.h>
#include <Arduino.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
// #include <.h>
#include <PubSubClient.h>
#include "effects/effects.h"
#include "utils/utils.h"
#include "commands/commands.h"

// LED Strip definition
#define NAME "neopixel"
#define PIN D2
#define DEBUG true //if enabled this increases the latency
#define LOG(...) \
    if (DEBUG)   \
    Serial.println(__VA_ARGS__)

const int EEPROM_ADDR = 0;
WiFiManager wm;
Adafruit_NeoPixel *strip;

int selectedEffect = 1;
boolean isEffectActive = true;

WiFiClient espClient;
PubSubClient client(espClient);

typedef struct
{
    char* topic = (char*)"esp8266/lights";
    char* responseTopic = (char *)"esp8266/lights/state";
    char* host = (char *)"berry.local";
    int port = 1883;
    int pixels = 144;
    bool enablePortal = false;
} Config;

Config config;

// Handle data definition
const size_t jsonReceiveDataCapacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;

void connectToBroker()
{
    if (!client.connected()) {
        while (!client.connected()) {
            LOG("Connecting to MQTT...");

            if (client.connect("esp8266-client")) {
                LOG("connected");
                client.subscribe(config.topic);
                String output = getStripState(strip, isEffectActive, selectedEffect);
                client.publish(config.responseTopic, const_cast<char*>(output.c_str()));
            }
            else {

                LOG("failed with state ");
                LOG(client.state());
                delay(1000);
            }
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {

    LOG("Message arrived in topic: " + (String)topic);
    DynamicJsonDocument doc(jsonReceiveDataCapacity);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        LOG(error.c_str());
    }
    else
    {
        String output = "{}";

        LOG("################");
        serializeJson(doc, Serial);
        LOG("\n----------------");
        
        switch (doc["c"].as<int>())
        {
        case 0:
            output = commandFill(strip, doc);
            break;
        case 1:
            output = setBrightness(strip, doc);
            break;
        case 2:
            strip->clear();
            break;
        case 3:
            output = toggleEffect(strip, doc, &isEffectActive, &selectedEffect);
            break;
        case 4:
            output = setPixels(strip, doc);
            break;
        case 5:
            output = getStripState(strip, isEffectActive, selectedEffect);
            break;
        }
        strip->show();
        LOG(output);
        client.publish(config.responseTopic, output.c_str());
    }
    LOG("################");
}

void saveWifiCallback()
{
    LOG("[CALLBACK] saveCallback fired");
    config.enablePortal = false;
    EEPROM.put(EEPROM_ADDR, config);
    EEPROM.commit();
}

void wifiInfo(WiFiManager *wm)
{
    WiFi.printDiag(Serial);
    Serial.println("SAVED: " + (String)wm->getWiFiIsSaved() ? "YES" : "NO");
    Serial.println("SSID: " + (String)wm->getWiFiSSID());
    Serial.println("PASS: " + (String)wm->getWiFiPass());
}

void logConfig(Config *config)
{
    LOG("### PIXELS: " + (String)config->pixels);
    LOG("### ENABLE PORTAL: " + (String)config->enablePortal);
    LOG("### HOST: " + (String)config->host);
    LOG("### ESPPORT: " + (String)config->port);
    LOG("### CON PATH: " + (String)config->topic);
    LOG("### Size: " + (String)sizeof(*config));
}

void setup()
{
    Serial.begin(115200);
    EEPROM.begin(512);
    delay(500);

    logConfig(&config);

    // std::vector<const char *> menu = {"wifi", "wifinoscan", "info", "param", "close", "sep", "erase", "restart", "exit"};

    WiFi.mode(WIFI_STA);                // explicitly set mode, esp defaults to STA+AP
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability
    wm.setClass("invert");
    wm.setCountry("US");
    wm.setHostname("esp8266.local");
    wm.setConfigPortalTimeout(120);
    wm.setConnectTimeout(60);
    wm.setBreakAfterConfig(true);
    wm.setSaveConfigCallback(saveWifiCallback);

    client.setServer(config.host, config.port);
    client.setCallback(callback);

    if (!wm.autoConnect() || config.enablePortal)
    {
        Serial.println("CONFIG ENABLED");
        wm.setConfigPortalTimeout(180);
        wm.startConfigPortal("WM_ConnectAP");
    }
    else
    {

        #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
        #endif

        strip = new Adafruit_NeoPixel(config.pixels, PIN, NEO_GRB + NEO_KHZ800);
        strip->begin();
        strip->setBrightness(255);

        delay(250);
        connectToBroker();
    }
    wifiInfo(&wm);
    delay(500);

    #ifdef USEOTA
    ArduinoOTA.begin();
    #endif
}

void loop()
{
    connectToBroker();
    client.loop();
    if (isEffectActive)
    {
        switch (selectedEffect)
        {
        case 1:
            rainbow(strip, 25);
            break;
        case 2:
            config.enablePortal = true;
            EEPROM.put(EEPROM_ADDR, config);
            EEPROM.commit();
            ESP.restart();
            break;
        case 3:
            meteorRain(strip, 0xff, 0xff, 0xff, 10, 64, true, 30);
            break;
        }
    }
}