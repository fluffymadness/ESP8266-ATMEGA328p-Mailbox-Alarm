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
        typedef std::function<bool(void)> readDeviceStateFunction;
        typedef std::function<void(bool)> switchDeviceStateFunction;
        typedef std::function<void(void)> restartFunction;
        typedef std::function<void(void)> restartFunctionSPIFF;
        typedef std::function<void(void)> onSettingsChange;
        typedef std::function<void(void)> onConfigResetFunction;
        typedef std::function<void(void)> onRebootFunction;
        typedef std::function<void(void)> onWifiConfigResetFunction;
        
        ESP8266WebServer server;
        ESP8266HTTPUpdateServer httpUpdater;
        readDeviceStateFunction mcb;
        switchDeviceStateFunction sdsf;
        restartFunction rfcb;
        restartFunctionSPIFF spiffcb;
        onSettingsChange oscf;
        onConfigResetFunction ocrf;
        onRebootFunction onrf;
        onWifiConfigResetFunction owcrf;
        String _temp; 
        String hardwareInfo();
        Settings *_settings; 
        Log *_syslog;
    public:
        MailCubeWebServer(Settings* settings, Log* syslog): _settings(settings),_syslog(syslog),server(80),httpUpdater(){}
        void begin();
        String getContentType(String);
        bool handleFileRead(String path);
        void registerReadDeviceStateCallback(readDeviceStateFunction function);
        void registerSwitchDeviceStateCallback(switchDeviceStateFunction function);
        void registerOnSpiffsUpdateCallback(restartFunctionSPIFF function);
        void registerOnRebootCallbackFunction(onConfigResetFunction function);
        void registerOnSettingsChangedCallback(onSettingsChange function);
        void registerOnConfigResetCallback(onConfigResetFunction function);
        void registerOnWifiConfigResetCallback(onWifiConfigResetFunction function);
        void handleClient();
    
};

#endif