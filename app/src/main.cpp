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

int selectedEffect = 4;
boolean isEffectActive = true;

WiFiClient espClient;
PubSubClient client(espClient);

typedef struct
{
    char* topicName =(char*)"mainStrip";
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

            if (client.connect(String("esp8266-client-" + (String)config.topicName).c_str())) {
                subscribe(&client, &config);
                String output;
                DynamicJsonDocument doc = getStripState(strip, isEffectActive, selectedEffect);
                doc["id"] = config.topicName;
                serializeJson(doc, output);
                publish(&client, &config, const_cast<char*>(output.c_str()));
                LOG("connected");
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
    wm.setHostname(String("esp8266-"+(String)config.topicName+".local").c_str());
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
        wm.startConfigPortal(String("WM_ConnectAP_" + (String)config.topicName).c_str());
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
        delay(250);
    }
    wifiInfo(&wm);
    delay(500);

    #ifdef USEOTA
    ArduinoOTA.begin();
    #endif
}

// breathing pixel variables
unsigned long startTime = 0;
int colorR = 150, colorG = 0, colorB = 200;
int breathTime = 9000; // 6000ms = six seconds; adjust for your desired speed
float breathTimeFloat = breathTime;
float pi = 3.14;

// initialize the breath timer
void startBreath() {
    startTime = millis();
    strip->fill(strip->Color(colorR, colorG, colorB));
    strip->show();
}

// call this inside your loop to update the color of the pixels
void updateBreath() {
    const float pi = 3.14;
    float frac; // ratio of color to use, based on time
    int r, g, b;

    // calculate a brightness fraction, based on a sine curve changing over time from 0 to 1 and back
    frac = (sin((((millis() - startTime) % breathTime)/breathTimeFloat - 0.25f) * 2.0f * pi) + 1.0f)/2.0f;

    // multiply each color by the brightness fraction
    r = colorR * frac;
    g = colorG * frac;
    b = colorB * frac;

    strip->fill(strip->Color(r, g, b));
    strip->show();
}

void loop()
{
    client.loop();
    if (isEffectActive)
    {
        switch (selectedEffect)
        {
        case 1:
            doubleRainbow(strip, 32);
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
        case 4:
            updateBreath();
            break;
        }
    }
    connectToBroker();
}
