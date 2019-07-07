#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include "ArduinoJson.h"
#include "Settings.h"
#include "Log.h"

#ifndef _MAILCUBE_WEB_SERVER
#define _MAILCUBE_WEB_SERVER

class MailCubeWebServer{
    private:
        
        ESP8266WebServer server;
        ESP8266HTTPUpdateServer httpUpdater;
        std::function<bool(void)> ReadDeviceStateCallback;
        std::function<void(bool)> SwitchDeviceStateCallback;
        std::function<void(void)> OnSpiffsUpdateCallback;
        std::function<void(void)> OnRebootCallback;
        std::function<void(void)> OnSettingsChangedCallback;
        std::function<void(void)> onConfigResetCallback;
        std::function<void(void)> OnWifiConfigResetCallback;
        String _temp; 
        String hardwareInfo();
        Settings *_settings; 
        Log *_syslog;
    public:
        MailCubeWebServer(Settings* settings, Log* syslog): _settings(settings),_syslog(syslog),server(80),httpUpdater(){}
        void begin();
        String getContentType(String);
        bool handleFileRead(String path);
        void registerReadDeviceStateCallback(std::function<bool(void)>);
        void registerSwitchDeviceStateCallback(std::function<void(bool)>);
        void registerOnSpiffsUpdateCallback(std::function<void(void)>);
        void registerOnRebootCallback(std::function<void(void)>);
        void registerOnSettingsChangedCallback(std::function<void(void)>);
        void registerOnConfigResetCallback(std::function<void(void)>);
        void registerOnWifiConfigResetCallback(std::function<void(void)>);
        void handleClient();
    
};

#endif