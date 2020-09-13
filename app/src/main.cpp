#include <stdio.h>
#include <EEPROM.h>
#include <Arduino.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
// #include <.h>
#include <PubSubClient.h>
#include "effects/effects.h"
#include "utils/utils.h"
#include "commands/commands.h"
#include "effects/breath.h"
#include "env.cpp"

// LED Strip definition
#define NAME "neopixel"
#define PIN D2
// #define DEBUG true //if enabled this increases the latency
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
    char *topicName = (char *)STR_VALUE(APP_TOPIC_NAME);
    char *topicPath = (char *)STR_VALUE(APP_TOPIC_PATH);
    char *host = (char *)STR_VALUE(APP_HOST);
    int port = APP_PORT;
    int pixels = APP_STRIP_PIXELS;
    bool enablePortal = APP_ENABLE_PORTAL;
    char *otaPassword = (char *)STR_VALUE(APP_OTA_PASSWORD);
} Config;

Config config;

BreathConfig breathConfig;
RainbowConfig rainbowConfig;

// Handle data definition
const size_t jsonReceiveDataCapacity = JSON_OBJECT_SIZE(8) + JSON_ARRAY_SIZE(4) + 60;

void publish(PubSubClient *client, Config *config, char *payload)
{
    client->publish(String((String)config->topicPath + "/status").c_str(), payload);
    client->publish(String((String)config->topicPath + "/" + (String)config->topicName + "/status").c_str(), payload);
}

void subscribe(PubSubClient *client, Config *config)
{
    client->subscribe(String((String)config->topicPath).c_str());
    client->subscribe(String((String)config->topicPath + "/" + (String)config->topicName).c_str());
}

void connectToBroker()
{
    if (!client.connected())
    {
        while (!client.connected())
        {
            LOG("Connecting to MQTT...");

            if (client.connect(String("esp8266-client-" + (String)config.topicName).c_str()))
            {
                subscribe(&client, &config);
                String output;
                DynamicJsonDocument doc = getStripState(strip, isEffectActive, selectedEffect);
                doc["id"] = config.topicName;
                serializeJson(doc, output);
                publish(&client, &config, const_cast<char *>(output.c_str()));
                LOG("connected");
            }
            else
            {

                LOG("failed with state ");
                LOG(client.state());
                delay(1000);
            }
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{

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
        case 6:
            createBreathConfig(
                &breathConfig,
                doc["s"].as<unsigned long>(),
                doc["r"].as<int>(),
                doc["g"].as<int>(),
                doc["b"].as<int>(),
                doc["t"].as<int>());
            isEffectActive = true;
            selectedEffect = 4;
            break;
        case 7:
            rainbowConfig.interval = doc["i"].as<unsigned long>();
            rainbowConfig.type = doc["t"].as<unsigned long>();
            isEffectActive = true;
            selectedEffect = 1;
            break;
        }
        strip->show();
        outputDoc["id"] = config.topicName;
        serializeJson(outputDoc, output);
        LOG(output);
        publish(&client, &config, const_cast<char *>(output.c_str()));
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
    LOG("SAVED: " + (String)wm->getWiFiIsSaved() ? "YES" : "NO");
    LOG("SSID: " + (String)wm->getWiFiSSID());
    LOG("PASS: " + (String)wm->getWiFiPass());
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
    wm.setHostname(String("esp8266-" + (String)config.topicName + ".local").c_str());
    wm.setConfigPortalTimeout(120);
    wm.setConnectTimeout(60);
    wm.setBreakAfterConfig(true);
    wm.setSaveConfigCallback(saveWifiCallback);

    client.setServer(config.host, config.port);
    client.setCallback(callback);

    if (!wm.autoConnect() || config.enablePortal)
    {
        LOG("CONFIG ENABLED");
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

        ArduinoOTA.setHostname(String("ESP8266" + (String)config.topicName).c_str());
        ArduinoOTA.setPassword(config.otaPassword);

        ArduinoOTA.onStart([]() {
            LOG("Start");
        });
        ArduinoOTA.onEnd([]() {
            LOG("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                LOG("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                LOG("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                LOG("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                LOG("Receive Failed");
            else if (error == OTA_END_ERROR)
                LOG("End Failed");
        });
        ArduinoOTA.begin();
        LOG("OTA ready");
    }
    wifiInfo(&wm);
    delay(500);
}

void loop()
{
    ArduinoOTA.handle();
    client.loop();
    if (isEffectActive)
    {
        switch (selectedEffect)
        {
        case 1:
            switch (rainbowConfig.type)
            {
            case 0:
                updateRainbowCycle(strip, &rainbowConfig);
                break;
            case 1:
                updateRainbow(strip, &rainbowConfig);
                break;
            case 2:
                updateDoubleRainbow(strip, &rainbowConfig);
                break;
            }
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
            updateBreath(strip, &breathConfig);
            break;
        }
    }
    connectToBroker();
}
