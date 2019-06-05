
#include "ArduinoJson.h"
#include "WiFiClient.h"
#include "Log.h"
#include "MyNTP.h"
#include "PubSubClient.h"
#ifndef MQTT_CLIENT
#define MQTT_CLIENT

class MQTTClient{

    private:  
        PubSubClient *client;    
        Log* _syslog;
        long generalTimer;
        long teleTimer;
        char msg[50];
        bool _initDone;
        String _deviceName;
        String _deviceTopicName;
        String _statPowerTopic;
        String _statPowerTopicH;
        String _statResultTopic;
        String _statResultTopicH;
        String _teleStateTopic;
        String _teleStateTopicH;
        String _cmndPowerTopic;
        String _cmndPowerTopicH;
        String _teleLWTTopic;
        String _teleLWTTopicH;
        String _homeAssistantTopic;
        String _homeAssistantID;
       
        String constructDiscoveryMessage();
        String getStateNode(bool value);
        void init();
        void callback(char* topic, uint8_t* payload, unsigned int length);
        void reconnect();

        typedef std::function<bool(void)> readDeviceStateFunction;
        typedef std::function<void(bool)> changeDeviceStateFunction;
        readDeviceStateFunction deviceStateFunction;
        changeDeviceStateFunction chDeviceStateFunction;
        
    
    public:
        void registerReadDeviceStateCallback(readDeviceStateFunction function);
        void registerToggleDeviceStateCallback(changeDeviceStateFunction function);
        void switchDevice(bool value);
        MQTTClient(
            IPAddress mqttIP,
            uint16_t port, 
            WiFiClient* espClient, 
            Log* log, 
            readDeviceStateFunction function, 
            changeDeviceStateFunction chFunction,
            String deviceName,
            String deviceTopicName
        ):
        _syslog(log),
        generalTimer(0),
        teleTimer(0),
        _initDone(false),
        deviceStateFunction(function),
        chDeviceStateFunction(chFunction),
        _deviceName(deviceName),
        _deviceTopicName(deviceTopicName){
          client = new PubSubClient();
          client->setServer(mqttIP, port);
          client->setClient(*espClient);
          using namespace std::placeholders;
          client->setCallback(std::bind(&MQTTClient::callback, this, _1,_2,_3));
          _teleStateTopicH = "tele/STATE";
          _statResultTopicH = "stat/RESULT";
          _statPowerTopicH = "stat/POWER";
          _cmndPowerTopicH = "cmnd/POWER";
          _teleLWTTopicH = "tele/LWT";
          _teleLWTTopic = deviceTopicName+"/tele/LWT";
          _teleStateTopic = deviceTopicName+"/tele/STATE";
          _statResultTopic = deviceTopicName+"/stat/RESULT";
          _statPowerTopic = deviceTopicName+"/stat/POWER";
          _cmndPowerTopic = deviceTopicName+"/cmnd/POWER";
          _homeAssistantID = "5DE191";
          _homeAssistantTopic = "homeassistant/switch/"+_homeAssistantID+"_RL_1/config";
        }
        void loop();
        ~MQTTClient(){
            delete client;
        }

};

#endif