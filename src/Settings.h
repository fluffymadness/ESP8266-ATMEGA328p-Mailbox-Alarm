#ifndef _SETTINGS
#define _SETTINGS

#include <ArduinoJson.h>
#include <FS.h>
#include "Log.h"
#include "MyNTP.h"
#include <sstream>

class Settings{
    private:
        
        String hostname;
        String mqttTopic;
        String mqttIP;
        int mqttPort;
        bool enableMQTT;
        String domoticzgeturl;
        int domoticzidx;
        bool enableDomoticzGet;
        bool deviceState;
        int bootCounter;
        Log *_syslog;

        template <typename T>
        void setIfPresent(JsonObject& obj, const char* key, T& var) {
            if (obj.containsKey(key)) {
                var = obj.get<T>(key);
            }
        }
        void fillJsonSettingsObject(JsonObject &root);

    public:
        Settings(Log* syslog):_syslog(syslog){}
        void readConfig();
        void writeConfig();
        void getSettings(JsonObject &root);
        bool setSettings(JsonObject &parsedSettings);
        String getHostname();
        IPAddress getMQTTIp();
        int getMQTTPort();
        void setHostname(String hostname); 
        void incrementBootCounter();
        int getBootCounter();
        void resetConfig();
        bool isMQTTEnabled();
        String getMQTTTopic();
        void setDeviceState(bool value);
        bool getDeviceState();
};

#endif