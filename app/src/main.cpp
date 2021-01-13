#include <stdio.h>
#include <EEPROM.h>
#include <Arduino.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WS2812FX.h>
#include <PubSubClient.h>
#include "utils/utils.h"
#include "commands/commands.h"
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
WS2812FX *strip;

WiFiClient espClient;
PubSubClient client(espClient);

typedef struct
{
    char *topicName = STR_VALUE(APP_TOPIC_NAME);
    char *topicPath = STR_VALUE(APP_TOPIC_PATH);
    char *host = STR_VALUE(APP_HOST);
    int port = APP_PORT;
    int pixels = APP_STRIP_PIXELS;
    int mode = APP_STRIP_MODE;
    uint32_t color = APP_STRIP_COLOR;
    int speed = APP_STRIP_SPEED;
    bool enablePortal = APP_ENABLE_PORTAL;
    char *otaPassword = STR_VALUE(APP_OTA_PASSWORD);
} Config;

Config config;

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

void updateStripConfig(WS2812FX *strip)
{
    config.color = strip->getColor();
    config.mode = strip->getMode();
    config.speed = strip->getSpeed();
    EEPROM.put(EEPROM_ADDR, config);
    EEPROM.commit();
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
        DynamicJsonDocument outputDoc(JSON_OBJECT_SIZE(8));
        String output;

        LOG("################");
        serializeJson(doc, Serial);
        LOG("\n----------------");

        switch (doc["c"].as<int>())
        {
        case 0:
            outputDoc = setStripColor(strip, doc);
            break;
        case 1:
            outputDoc = setBrightness(strip, doc);
            break;
        case 3:
            outputDoc = setStripEffect(strip, doc);
            break;
        case 4:
            outputDoc = setStripSpeed(strip, doc);
            break;
        case 5:
            // status update is default to all calls or 5
            break;
        case 99:
            // reset config
            Config default_config;
            EEPROM.put(EEPROM_ADDR, default_config);
            EEPROM.commit();
        }
        outputDoc = getStripState(strip, doc);
        outputDoc["id"] = config.topicName;
        serializeJson(outputDoc, output);
        LOG(output);
        publish(&client, &config, const_cast<char *>(output.c_str()));
        updateStripConfig(strip);
    }
    LOG("################");
}

void connectToBroker()
{
    if (!client.connected())
    {
        while (!client.connected())
        {
            LOG("Connecting to MQTT...");

            //if (client.connect(String("esp8266-client-" + (String)config.topicName).c_str()))
            String clientId = "ESP8266Client-";
            clientId += String(random(0xffff), HEX);
            if (client.connect(clientId.c_str()))
            {
                subscribe(&client, &config);
                String output;
                DynamicJsonDocument doc(jsonReceiveDataCapacity);
                doc = getStripState(strip, doc);
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
    EEPROM.begin(4096);
    delay(500);
    // EEPROM.put(EEPROM_ADDR, config);
    logConfig(&config);

    EEPROM.get(EEPROM_ADDR, config);
    logConfig(&config);

    // std::vector<const char *> menu = {"wifi", "wifinoscan", "info", "param", "close", "sep", "erase", "restart", "exit"};

    WiFi.mode(WIFI_STA);                // explicitly set mode, esp defaults to STA+AP
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability
    WiFi.hostname(String("esp8266-" + (String)config.topicName + ".local").c_str());
    wm.setHostname(String("esp8266-" + (String)config.topicName + ".local").c_str());
    wm.setClass("invert");
    wm.setCountry("US");
    wm.setConfigPortalTimeout(120);
    wm.setConnectTimeout(60);
    wm.setBreakAfterConfig(true);
    wm.setSaveConfigCallback(saveWifiCallback);
    wm.setCleanConnect(true);

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

        LOG("Configuring client");
        client.setServer(config.host, config.port);
        client.setCallback(callback);
        delay(500);

        strip = new WS2812FX(config.pixels, PIN, NEO_GRB + NEO_KHZ800);
        strip->init();
        strip->setBrightness(255);
        strip->setSpeed(config.speed);
        strip->setColor(config.color);
        strip->setMode(config.mode);
        strip->start();

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

        delay(250);
        connectToBroker();
        delay(250);
    }
    wifiInfo(&wm);
    delay(500);
}

void loop()
{
    ArduinoOTA.handle();
    client.loop();
    strip->service();
    connectToBroker();
}