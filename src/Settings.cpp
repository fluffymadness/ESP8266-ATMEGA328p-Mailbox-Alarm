#include "Settings.h"

void Settings::readConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(configFile);
  
  if (!root.success()){
    _syslog->addMessage("Failed to read config file /config.json from SPIFFS, using default values");
  }
  else{
    _syslog->addMessage("Successfully read /config.json from SPIFFS, using default values");
  }

  hostname = root["hostname"] | "SeanetCube";
  mqttTopic = root["mqttTopic"] | "cube";
  mqttIP = root["mqttIP"] | "";
  mqttPort = root["mqttPort"] | 1883;
  enableMQTT = root["enableMQTT"] | true;
  domoticzidx = root["domoticzidx"] | 0;
  domoticzgeturl = root["domoticzgeturl"] | "";
  enableDomoticzGet = root["enableDomoticzGet"] | true;
  bootCounter = root["bootCounter"] | 0;
  deviceState = root["deviceState"] | false;
  configFile.close();
}
void Settings::writeConfig() {

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    _syslog->addMessage("Could not open config file in SPIFFS");
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  fillJsonSettingsObject(root);
  // Serialize JSON to file
  if (root.printTo(configFile) == 0) {
    _syslog->addMessage("Could not store config in SPIFFS");
  }

  configFile.close();
}
void Settings::fillJsonSettingsObject(JsonObject &root){
  root["hostname"] = hostname;
  root["mqttTopic"] = mqttTopic;
  root["mqttIP"] = mqttIP;
  root["mqttPort"] = mqttPort;
  root["enableMQTT"] = enableMQTT;
  root["domoticzidx"] = domoticzidx;
  root["domoticzgeturl"] = domoticzgeturl;
  root["enableDomoticzGet"] = enableDomoticzGet;
  root["bootCounter"] = bootCounter;
  root["deviceState"] = deviceState;
}
void Settings::getSettings(JsonObject &root){
    fillJsonSettingsObject(root);
}
bool Settings::setSettings(JsonObject& parsedSettings) {
  if (parsedSettings.success()) {
    this->setIfPresent<String>(parsedSettings, "hostname", hostname);
    this->setIfPresent<String>(parsedSettings, "mqttTopic", mqttTopic);
    this->setIfPresent<String>(parsedSettings, "mqttIP", mqttIP);
    this->setIfPresent(parsedSettings, "mqttPort", mqttPort);
    this->setIfPresent<bool>(parsedSettings, "enableMQTT", enableMQTT);
    this->setIfPresent(parsedSettings, "domoticzidx", domoticzidx);
    this->setIfPresent<String>(parsedSettings, "domoticzgeturl", domoticzgeturl);
    this->setIfPresent<bool>(parsedSettings, "enableDomoticzGet", enableDomoticzGet);
    writeConfig();
    return true;
  }
  else{
    return false;
  }
}
bool Settings::isMQTTEnabled(){
  return enableMQTT;
}
String Settings::getHostname(){
  return hostname;
}
IPAddress Settings::getMQTTIp(){
  IPAddress temp;
  temp.fromString(mqttIP);
  return temp;
}
String Settings::getMQTTTopic(){
  return mqttTopic;
}
int Settings::getMQTTPort(){
  return mqttPort;
}
void Settings::setHostname(String hostname){
  hostname=hostname;
  writeConfig();
}
void Settings::setDeviceState(bool value){
  deviceState = value;
  writeConfig();
}
bool Settings::getDeviceState(){
  return deviceState;
}
void Settings::incrementBootCounter(){
  bootCounter++;
  writeConfig();
}
int Settings::getBootCounter(){
  return bootCounter;
}
void Settings::resetConfig(){
      _syslog->addMessage("deleting file /config.json");
      SPIFFS.remove("/config.json");
}
