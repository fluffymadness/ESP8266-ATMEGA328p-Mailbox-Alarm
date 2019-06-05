# SeanetMailbox

## Introduction

This is one of my work in progress personal projects.
The goal is to have a client and a server node for sending and receiving status
of my physical mailbox outside.

The client node consists of an atmega328 microcontroller, a a nordic semi conductor 2.4ghz transceiver (nrf24l01) and a magnetic reed switch.
The Atmega should be running at a reduced clockrate and be driven by a coincell (CRC2032 3V) to last several months.
To realize this, the mcu has to be in deep sleep mode all the time and should only wake up when the mailbox gets opened.

After waking up from deepsleep (when the mailbox gets opened) the signal should be transmitted instantly and be sent to the server node.

The server node consists of an esp8266 with a led and another nrf24l01 attached to it.
When the the server receives the message it should deliver a message via mqtt to an attached home automation system (e.g Domoticz or Homeassistant).
Also the webinterface should be updated accordingly.

Resetting the state, e.g when the mail was fetched should then be possible via webinterface, mqtt commands, or a simple button press on the server node.

## Screenshots of the Webinterface

![Alt text](status.jpg?raw=true "Status")
![Alt text](config.jpg?raw=true "Config")
![Alt text](system.jpg?raw=true "System")
![Alt text](info.jpg?raw=true "Info")

# Already Implemented Functionality

## Server

### General
- Working Wifi Captivate Portal for initial Connection to a HomeAccessPoint when the device is first initialized or when the reset button is hold during bootup to reset the Device state.
- Proper Handling of the Reset Button on bootup to reset the device state to factory settings
- NTP Server for getting the correct Time via the Internet for Logs
- Debug Interface (Logging of all Operations on the Device via Serial Output and Web Interface)
- Pushing the actual device status via mqtt Messages to a HomeAutomation System like Domoticz or Home Assistant

### Webinterface

- Complete Webinterface realized with Bootstrap
- Status page to see if the mailbox is empty or full realized via pictures and text.
- Firmware can be upgraded via the WebInterface
- Single files of the webinterface can be changed via form upload
- Complete filesystem of the webinterface can be changed via a complete SPIFFS upload
- Maintenence Functions (Reboot, ResetWifiSettings, Reset Complete Config)
- Editable Config via Webinterface (Hostname, MQTT-Broker-Ip, MQTT-broker-Port, MQTT-Topic, Domoticz Settings for Virtual Device IDX And custom Domoticz GET Url)
- Info Page for inspecting Logs of the device and seeing Hardware Overview Like Ip Address, Mac Address etc of the device

# Todo 

## Server
- Implementing a fix for the slow and unresponsive webinterface when mqtt is enabled and the broker is not found
- Cleanup the Domoticz Settings since there is a autodiscovery plugin for homeassistant style mqtt messages for domoticz.
- Implementation of the Nordic Semiconductor Receiver part to receive the messages from the client node.

## Client

- Getting the ATMega328 in Low-Power-State (1.8v) as described here: 
https://www.iot-experiments.com/arduino-pro-mini-1mhz-1-8v/
- Building a Simple Circuit with a MagneticReedSwitch and a Coincell
- Writing the Client Part for communication with the server node via the nrf24l01

## Libraries used in the C/C++ Part of the Project

- PubSubClient
- ArduinoJson.h
- NtpClientLib
- ESP8266WebServer
- ESP8266HTTPUpdateServer
- ESP8266mDNS
- Bounce2
- Arduino
- DNSServer
- WifiManager

## Libraries/Frameworks used for the WebInterface

- Bootstrap
- JQuery
- Popper
