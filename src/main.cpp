#include "Arduino.h"
#include <stdio.h>
#include "FS.h"
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "MailCubeWebServer.h"
#include "Log.h"
#include "MyNTP.h"
#include "MQTTClient.h"
#include "Bounce2.h"

/**Program Logic**/
#define LEDPIN D7
#define PUSHBUTTONPIN D6
#define RESETPIN D5

enum mcuState {
  BOOTING,
  RUNNING,
  RESET,
  INIT,
  NOTHING
};

#define ON true
#define OFF false
bool deviceState=false;

Bounce debouncer = Bounce();  
enum mcuState state;
String ntpError;
DNSServer dns;
Log* syslog = NULL;
MyNTP* ntp = NULL;
Settings* settings = NULL;
MailCubeWebServer* mc = NULL;
MQTTClient* mqttClient = NULL;
WiFiClient* espClient = NULL;


void initMain(){
  syslog = new Log(&ntp);
  settings = new Settings(syslog);
  espClient = new WiFiClient();
}
void restartMcu(){
    delay(3000);
    ESP.reset();
    delay(2000);
}
void resetWifiConfig(){
    syslog->addMessage("Resetting WifiManager");
    delay(2000);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
}
void wifiPortal(){
  syslog->addMessage("Connection to saved access Point...");
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  //first parameter is name of access point, second is the password
  if(!wifiManager.autoConnect(settings->getHostname().c_str(),"cube")){
      syslog->addMessage("failed to connect, we should reset as see if it connects");
      delay(3000);
      ESP.reset();
      delay(5000);
  }
  syslog->addMessage("Connection Success, IP Address: "+WiFi.localIP().toString());
}
void setupPins(){
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);
  pinMode(RESETPIN,INPUT_PULLUP);
  debouncer.attach(PUSHBUTTONPIN,INPUT_PULLUP);
  debouncer.interval(100);
}

void setup() {
  setupPins();
  Serial.begin(115200);
  initMain();
  syslog->addMessage("Power on");
  state=BOOTING;
}
bool isResetPinPressed(){if(digitalRead(RESETPIN)==LOW){return true;} return false;}
void setLed(int state){digitalWrite(LEDPIN,state);}
void switchDevice(bool _state){
  syslog->addMessage(_state ? "Set Switch to ON" : "Set Switch to OFF"); //why no works?
  deviceState = _state;
  setLed(_state);
  if(mqttClient != nullptr)
    mqttClient->switchDevice(_state);
  settings->setDeviceState(_state);
}

void toggleDevice(){
  if(deviceState == ON){
    switchDevice(OFF);
  }
  else{
    switchDevice(ON);
  }  
}

void initWebserver(){
    if(mc==nullptr){
      syslog->addMessage("Initializing Webserver");

      mc = new MailCubeWebServer(settings,syslog);
      mc->registerOnSettingsChangedCallback([](){
        restartMcu();
      });
      mc->registerReadDeviceStateCallback([](){
        if(deviceState == ON)
          return true;
        else 
          return false;
      });
      mc->registerSwitchDeviceStateCallback([](bool _state){
          switchDevice(_state);
      });

      mc->registerOnSpiffsUpdateCallback([](){});
      mc->registerOnRebootCallback([](){
        restartMcu();
      });
      mc->registerOnConfigResetCallback([](){
        settings->resetConfig();
        restartMcu();
      });
      mc->registerOnWifiConfigResetCallback(resetWifiConfig);
      mc->begin();
    }
    else{
      
    }
}
void initNTP(){
    syslog->addMessage("Initializing NTP Server");
    ntp = new MyNTP();
    ntp->initNtp(1,0);
}
void initMQTT(){
   if(settings->isMQTTEnabled()){
      syslog->addMessage("Initialize MQTT Client");
      mqttClient= new MQTTClient(
        settings->getMQTTIp(),
        settings->getMQTTPort(), 
        espClient, 
        syslog,
        []()
        {
          if(deviceState == ON)
            return true;
          else 
            return false; 
        },
        switchDevice,
        settings->getHostname(),
        settings->getMQTTTopic()
      );
    }
}
void loop() {
  switch (state) {
      case BOOTING:
            syslog->addMessage("Initializing SPIFFS...");
            if(!SPIFFS.begin()==true){
              syslog->addMessage("An Error has occurred while mounting SPIFFS");
            }
            else{
              settings->readConfig();
              syslog->addMessage("Booting...");
              settings->incrementBootCounter();
              if(isResetPinPressed()){
                state=RESET;
              }
              else{
                state=INIT;
              }
            }      
        break;
      case INIT:
              WiFi.hostname(settings->getHostname());
              wifiPortal();
              initNTP();
              initMQTT();
              initWebserver();
              switchDevice(settings->getDeviceState()); //Resore Device State
              syslog->addMessage("Boot ok");
              state=RUNNING;
        break;
      case RUNNING:
          debouncer.update();
          if (debouncer.fell()){
              toggleDevice(); //Handle Pushbutton
          }

          ntpError = ntp->tick(); //NTP Server
          if(ntpError !="OK"){
              syslog->addMessage(ntpError);
          }

          if(settings->isMQTTEnabled()){
           // mqttClient->loop();  //MQTT Server
          }

          mc->handleClient(); //Handle WebserverClient
        break;
      case RESET:
          resetWifiConfig();
          settings->resetConfig();
          delay(2000);
          restartMcu();
        break;
      default:
        // statements
        break;
  }
  
}

