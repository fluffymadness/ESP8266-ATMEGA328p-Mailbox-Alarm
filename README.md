# SeanetMailbox

## Introduction

This project is a WIP. The recommended parts, microcontrollers, types of battery etc could still change later on.

The goal is to have a client and a server node for receiving status information
from my physical mailbox outside.
The server part of the project is running currently on a NodeMCUv3 board for testing.

The planned client node consists of an atmega328 microcontroller, a a nordic semi conductor 2.4ghz transceiver (nrf24l01) and a magnetic reed switch.
The Atmega should be running at a reduced clockrate and be driven by a coincell (CRC2032 3V) to last several months.
To realize this, the mcu has to be in deep sleep mode all the time and should only wake up when the mailbox gets opened.
After waking up from deepsleep the signal should be transmitted instantly to the server node indicating that the mailbox has been opened.

The server node consists of an esp8266 with a led and has a nrf24l01 attached to it.
When the the server realizes that the mailbox has been opened it should deliver a message via mqtt to an attached home automation system (e.g Domoticz or Homeassistant).
Also the status of the Info-Page on the webinterface should update accordingly.

Resetting the actual state, e.g when the mail was fetched should then be possible via the webinterface, mqtt commands, or a simple button press on the server node.

## Parts Overview

Parts can be cheaply sourced from aliexpress. e.g

ATMEGA328 module (get the 3.3v one) for the client 
(https://www.aliexpress.com/item/1pcs-lot-Pro-Mini-Module-Atmega328-5V-16M-with-Arduino-Compatible-With-Nano/32672852945.html?spm=2114.search0104.3.1.4b313252EfP7qk&ws_ab_test=searchweb0_0,searchweb201602_6_10065_10068_10547_319_317_10548_10696_10084_453_10083_454_10618_10304_10307_10820_10821_537_10302_536_10059_10884_10887_321_322_10103,searchweb201603_52,ppcSwitch_0&algo_expid=28464f61-92ab-41cf-9391-0c138483a240-0&algo_pvid=28464f61-92ab-41cf-9391-0c138483a240&transAbTest=ae803_3)

NodeMCU board for the server(
https://www.aliexpress.com/item/V2-4M-4FLASH-NodeMcu-Lua-WIFI-Networking-development-board-Based-ESP8266/32647690484.html?spm=2114.search0104.3.3.6c445c45bPX0AV&ws_ab_test=searchweb0_0,searchweb201602_6_10065_10068_10547_319_317_10548_10696_10084_453_10083_454_10618_10304_10307_10820_10821_537_10302_536_10059_10884_10887_321_322_10103,searchweb201603_52,ppcSwitch_0&algo_expid=ca9ed334-9f93-40da-b08b-114a98e12ee5-0&algo_pvid=ca9ed334-9f93-40da-b08b-114a98e12ee5&transAbTest=ae803_3)

The rest are standard parts that can also be sourced from aliexpress
- Magnetic Reed Switch (still have to look what to get, Normally-Opened or Normally-Closed in terms of powermanagement for the microcontroller)
- LED

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
