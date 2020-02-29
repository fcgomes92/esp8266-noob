#define DEBUG_WEBSOCKETS true
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

const char *ssid = "";
const char *password = "";

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

int pinToRgb(int val)
{
  return PWMRANGE - (val * PWMRANGE) / 255;
}

void toRgb(int red, int green, int blue)
{
  int r = pinToRgb(red);
  int g = pinToRgb(green);
  int b = pinToRgb(blue);
  Serial.printf("R: %u | G: %u - | B: %u \n\n", r, g, b);
  analogWrite(D2, r);
  analogWrite(D3, g);
  analogWrite(D4, b);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
  {
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    Serial.println();

    // send message to server when Connected
    webSocket.sendTXT("Connected");
  }
  break;
  case WStype_TEXT:
    Serial.printf("[WSc] get text: %s\n", payload);
    Serial.println();

    // send message to server
    webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    Serial.printf("[WSc] get binary length: %u\n", length);
    Serial.println();
    hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length);
    break;
  }
}

void setTime()
{

  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setupLeds()
{
  toRgb(0, 0, 0);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.println("");
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP(ssid, password);

  Serial.println("Connecting WiFi...");
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(100);
  }

  Serial.println("WiFi Connected");
  String ip = WiFi.localIP().toString();
  Serial.printf("[WiFi] WiFi Connected %s\n", ip.c_str());
  Serial.println();

  setTime();

  Serial.println("Connecting to the WebSocket");
  webSocket.begin("192.168.0.110", 8080, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  // webSocket.enableHeartbeat(15000, 3000, 2);
}

void loopLeds()
{
  Serial.println("red");
  toRgb(255, 0, 0);
  delay(150);

  Serial.println("Blue");
  toRgb(0, 0, 255);
  delay(150);

  Serial.println("Green");
  toRgb(0, 255, 0);
  delay(150);
  
  Serial.println("Purple");
  toRgb(128, 0, 128);
  delay(5000);
}

void loop()
{
  webSocket.loop();
  loopLeds();
}
