#include "MQTTClient.h"

void MQTTClient::init(){
  if((!_initDone)&&(client->connected())){
      _syslog->addMessage("Sending Homeassistant Discovery Retained Message");
      client->publish(_homeAssistantTopic.c_str(),constructDiscoveryMessage().c_str(),true);
      _syslog->addMessage("Sending Tele LWT Retained Message");
      client->publish(_teleLWTTopic.c_str(),"Online",true); //TODO: Fix blinking on off bug in domoticz when last will message is set to offline
      _initDone = true;
  }
}
void MQTTClient::callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic,_cmndPowerTopic.c_str())==0){
    String payloadS="";
    for (int i = 0; i < length; i++) {
      payloadS+=(char)payload[i];
    }
    //tring s(payload, sizeof(payload));
    _syslog->addMessage("MQTT Topic Recieved: "+String(topic));
    _syslog->addMessage("MQTT Payload received: "+payloadS);
    if(payloadS=="ON"){
        chDeviceStateFunction(true);
    }
    else{
        chDeviceStateFunction(false);
    }

  }
}
void MQTTClient::reconnect() {
    _syslog->addMessage("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client->connect(clientId.c_str())) {
      _syslog->addMessage("MQTT Client connected");
      client->subscribe(_cmndPowerTopic.c_str());
    } else {
      _syslog->addMessage("MQTT Connection failed, rc="+String(client->state()));
    }
}
void MQTTClient::loop() {

  long now = millis();
 
  if (now - generalTimer > 4000) {
      generalTimer = now;
      if (!client->connected()) {
          reconnect();
      }
      init();
  }
  if (now - teleTimer > 20000){
      teleTimer = now;
      client->publish(_teleStateTopic.c_str(),getStateNode(deviceStateFunction()).c_str());
      _syslog->addMessage("Sending MQTT Tele Message");
  }
}
String MQTTClient::constructDiscoveryMessage(){

    String payload="";
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["name"] = _deviceName;
    root["cmd_t"] = "~"+_cmndPowerTopicH;
    root["stat_t"] = "~"+_teleStateTopicH;
    root["val_tpl"] = "{{value_json.POWER}}";
    root["pl_off"] = "OFF";
    root["pl_on"] = "ON";
    root["avty_t"] = "~"+_teleLWTTopicH;
    root["pl_avail"] = "Online";
    root["pl_not_avail"] = "Offline";
    root["uniq_id"] = _homeAssistantID+"_RL_1";
    JsonObject& device = root.createNestedObject("device");
    JsonArray& identifiersarray = device.createNestedArray("identifiers");
    identifiersarray.add(_homeAssistantID);
    device["name"] = _deviceName;
    device["model"] = "ESP8266";
    device["sw_version"] = "1.0.0.0";
    device["manufacturer"] = "fluffymadness";
    root["~"] = _deviceTopicName+"/";
    root.printTo(payload);
   // _syslog->addMessage(payload);
    return(payload);
}
void MQTTClient::switchDevice(bool value){
    String stateNode = getStateNode(value);
    client->publish(_teleStateTopic.c_str(),stateNode.c_str());
    client->publish(_statResultTopic.c_str(),stateNode.c_str());
    client->publish(_statPowerTopic.c_str(),value ? "ON" : "OFF");
}
String MQTTClient::getStateNode(bool value){
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["POWER"] = value ? "ON" : "OFF";
    String temp2;
    root.printTo(temp2);
    return temp2;
}
