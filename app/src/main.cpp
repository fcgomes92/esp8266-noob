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
    char* topicName =(char*)"main-strip";
    char* topicPath  = (char*)"office/lights";
    char* host = (char *)"berry.local";
    int port = 1883;
    int pixels = 144;
    bool enablePortal = false;
} Config;

Config config;

// Handle data definition
const size_t jsonReceiveDataCapacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;

void publish(PubSubClient *client, Config *config, char * payload) {
    client->publish(String((String)config->topicPath + "/status").c_str(), payload);
    client->publish(String((String)config->topicPath + "/" + (String)config->topicName + "/status").c_str(), payload);
}

void subscribe(PubSubClient *client, Config *config) {
    client->subscribe(String((String)config->topicPath).c_str());
    client->subscribe(String((String)config->topicPath + "/" + (String)config->topicName).c_str());
}

void connectToBroker()
{
    if (!client.connected()) {
        while (!client.connected()) {
            LOG("Connecting to MQTT...");

            if (client.connect("esp8266-client")) {
                LOG("connected");
                subscribe(&client, &config);
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
        DynamicJsonDocument outputDoc(JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(144) + JSON_OBJECT_SIZE(1));
        String output;

        LOG("################");
        serializeJson(doc, Serial);
        LOG("\n----------------");

        switch (doc["c"].as<int>())
        {
        case 0:
            outputDoc = commandFill(strip, doc);
            break;
        case 1:
            outputDoc = setBrightness(strip, doc);
            break;
        case 2:
            strip->clear();
            break;
        case 3:
            outputDoc = toggleEffect(strip, doc, &isEffectActive, &selectedEffect);
            break;
        case 4:
            outputDoc = setPixels(strip, doc);
            break;
        case 5:
            outputDoc = getStripState(strip, isEffectActive, selectedEffect);
            break;
        }
        strip->show();
        outputDoc["id"] = config.topicName;
        serializeJson(outputDoc, output);
        LOG(output);
        publish(&client, &config, const_cast<char*>(output.c_str()));
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
    LOG("### BROKER HOST: " + (String)config->host);
    LOG("### PORT: " + (String)config->port);
    LOG("### TOPIC NAME: " + (String)config->topicName);
    LOG("### TOPIC PATH: " + (String)config->topicPath);
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