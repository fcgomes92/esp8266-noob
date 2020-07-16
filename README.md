# esp8266-noob

## Current test interface

![test interface](./assets/test.png)

## Inner projects

There is the ESP8266 `app` code. This code is sent to the ESP8266 using the Arduino IDE interface. It controls a LED strip.

There is the `server` code. Responsible for connection w/ ESP8266 and local network. Handles communication between `app`s and `client`s.

There is the `client` code. Responsible for creating a user test interface. Communicates w/ the server / api.

## Hardware

- WS2812 LED strip. 144 LEDs in 1m. (60mA / LED)
- ESP8266 NodeMCU Module
- Power supply:
  - 5V 10A
- Capacitors:
  - 1x 470µF
- Resistors:
  - 1x 470Ω

## Prototype & Result

![prototype](./assets/prototype.jpeg)
