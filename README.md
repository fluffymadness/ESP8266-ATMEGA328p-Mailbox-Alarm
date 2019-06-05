# SeanetMailbox

## Introduction

This is one of my work-in-progress personal projects.
The goal is to have a client and a server node for sending and receiving status information
of my physical mailbox outside.

The client node consists of an atmega328 microcontroller, a a nordic semi conductor 2.4ghz transceiver (nrf24l01) and a magnetic reed switch.
The Atmega should be running at a reduced clockrate and be driven by a coincell (CRC2032 3V) to last several months.
To realize this, the mcu has to be in deep sleep mode all the time and should only wake up when the mailbox gets opened.
After waking up from deepsleep the signal should be transmitted instantly to the server node indicating that the mailbox has been opened.

The server node consists of an esp8266 with a led and has a nrf24l01 attached to it.
When the the server realizes that the mailbox has been opened it should deliver a message via mqtt to an attached home automation system (e.g Domoticz or Homeassistant).
Also the status of the Info-Page on the webinterface should be updated accordingly.

Resetting the actual state, e.g when the mail was fetched should then be possible via the webinterface, mqtt commands, or a simple button press on the server node.

## Screenshots of the Webinterface

![Alt text](status.jpg?raw=true "Status")
![Alt text](config.jpg?raw=true "Config")
![Alt text](system.jpg?raw=true "System")
![Alt text](info.jpg?raw=true "Info")

# Already Implemented Functionality

## Server

### General
- Working Wifi Captive Portal for the initial connection to a Home-Access-Point when the device is first initialized or when the reset button is held during bootup for resetting the state of the device
- Proper Handling of the Reset Button on bootup to reset the device state to factory settings
- NTP Server for getting the correct time via the internet for Logs
- Debug Interface (Logging of all operations on the device via Serial-Output and webinterface)
- Pushing the actual device status via MQTT-Messages to a Home-Automation-System like Domoticz or Home Assistant

### Webinterface

- Complete webinterface realized via Bootstrap
- Status page to see if the mailbox is empty or full realized via pictures and text.
- Firmware can be upgraded via webinterface
- Single files of the webinterface can be changed via form upload
- Complete filesystem for the webinterface can be changed via a complete SPIFFS upload
- Maintenence Functions (Reboot, Reset Wifi Settings, Reset Complete Config)
- Editable Config via webinterface (Hostname, MQTT-Broker-Ip, MQTT-broker-Port, MQTT-Topic, Domoticz Settings for Virtual Device IDX And custom Domoticz GET Url)
- Info Page for inspecting Logs of the device and for seeing Hardware Overview Information like Ip Address, Mac Address etc of the device

# Todo 

## Server
- Implementing a fix for the slow and unresponsive webinterface when MQTT is enabled and the broker is not found
- Cleanup the Domoticz Settings since there is a autodiscovery plugin for homeassistant style mqtt messages for domoticz.
- Implementation of the Nordic Semiconductor Receiver part to receive the messages from the client node.

## Client

- Getting the ATMega328 in Low-Power-State (1.8v) as described here: 
https://www.iot-experiments.com/arduino-pro-mini-1mhz-1-8v/
- Building a simple circuit with a magnetic reedswitch and a coincell
- Writing the client part for communication with the server node via the nrf24l01

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
