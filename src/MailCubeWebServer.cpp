#include "MailCubeWebServer.h"

extern "C" {
#include "spi_flash.h"
}
extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;


void MailCubeWebServer::begin(){

  server.onNotFound([this](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.on("/setConfig", HTTP_POST, [this](){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    String temp;
    root.printTo(temp);
    _syslog->addMessage(temp);
    if(_settings->setSettings(root)){
      _syslog->addMessage("Config saved..Device is Rebooting");
      server.send(200, "text/plain", "Config saved..Device is Rebooting");
      oscf();
    }
    else{
      _syslog->addMessage("Config-Saving error");
      server.send(404, "text/plain", "Config-Saving error");
    }
  });

  server.on("/getLog", HTTP_GET, [this](){
    server.send(200, "text/plain", _syslog->toString());  
  });
  server.on("/getInfo", HTTP_GET, [this](){
      server.send(200, "text/json", hardwareInfo());
  });
  server.on("/getHostname", HTTP_GET, [this](){
      server.send(200, "text/plain", _settings->getHostname());
  });
  server.on("/reboot", HTTP_GET, [this](){
      server.send(200, "text/plain", "System Rebooting");
      onrf();
  });
  server.on("/resetConfig", HTTP_GET, [this](){
      _syslog->addMessage("Resetting Config, the System restarts now");
      server.send(200, "text/plain", "Resetting Config, the System restarts now");
      ocrf();
  });
  server.on("/resetWifiConfig", HTTP_GET, [this](){
       _syslog->addMessage("Resetting Wifi Config, the System restarts now");
      server.send(200, "text/plain", "Resetting Wifi Config, the System restarts now");
      owcrf();
  });
  server.on("/getConfig", HTTP_GET, [this](){
      String json;
      DynamicJsonBuffer jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      _settings->getSettings(root);
      root.printTo(json);
      server.send(200, "text/json", json);
  });


  server.on("/uploadFile", HTTP_POST, [this](){
      server.send(200, "text/plain", "File uploaded");
  },[this](){
        HTTPUpload& upload = server.upload();
        static File tempfile;

        if(upload.status == UPLOAD_FILE_START){
            String filename = upload.filename;
            filename = "/"+filename;
            tempfile = SPIFFS.open(filename, "w");
        } 
        else if(upload.status == UPLOAD_FILE_WRITE) {
          if(tempfile)
            tempfile.write(upload.buf, upload.currentSize);
        } 
        else if(upload.status == UPLOAD_FILE_END) {
          if(tempfile)
            tempfile.close();
        }
  }); 
  httpUpdater.setup(&server, "/update");  
  server.begin();
}
void MailCubeWebServer::registerOnRebootCallbackFunction(onRebootFunction function){
    onrf=function;
}
void MailCubeWebServer::registerOnSettingsChangedCallback(onSettingsChange function){
    oscf=function;
}
void MailCubeWebServer::registerOnConfigResetCallback(onConfigResetFunction function){
    ocrf=function;
}
void MailCubeWebServer::registerOnWifiConfigResetCallback(onWifiConfigResetFunction function){
    owcrf=function;
}


void MailCubeWebServer::registerOnSpiffsUpdateCallback(restartFunctionSPIFF function){
    spiffcb = function;
    server.on("/uploadSPIFF", HTTP_POST, [this](){
      server.send(200, "text/plain", "File uploaded");
    },[this](){
    HTTPUpload& upload = server.upload(); 
   
    if(upload.status == UPLOAD_FILE_START)
    {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());       
    }
    else if(upload.status == UPLOAD_FILE_WRITE)
    {
        uint32_t startSpiffs = (uint32_t)&_SPIFFS_start - 0x40200000;
        uint32_t _sector = (startSpiffs + upload.totalSize)/ SPI_FLASH_SEC_SIZE;
        uint32_t data = (uint32_t)upload.buf;
       
        Serial.printf("Location: 0x%x, TotalSize: %d", startSpiffs + upload.totalSize, upload.totalSize);

        if((startSpiffs + upload.totalSize) % FLASH_SECTOR_SIZE == 0 )
        {
            if (spi_flash_erase_sector(_sector) == SPI_FLASH_RESULT_OK)
            {
                Serial.print(", Erased");
            }
            else
                Serial.print(", ErrorErase");
        }
       
        if (spi_flash_write(startSpiffs + upload.totalSize, reinterpret_cast<uint32_t*>(data), upload.currentSize) == SPI_FLASH_RESULT_OK)
          Serial.print(", Written");
        else
        Serial.print(", ErrorWritten");
        Serial.print("\n");
       // _sector++;
    }
    else if(upload.status == UPLOAD_FILE_END)
    {
        _syslog->addMessage("Successfully uploaded new SPIFFS Image: "+upload.totalSize);
        Serial.setDebugOutput(false);
    }
   
    yield();
    });
}
void MailCubeWebServer::registerReadDeviceStateCallback(readDeviceStateFunction function){
      server.on("/readMailState",HTTP_GET, [this](){
          if(mcb()){
            server.send(200, "text/plane", "1");
          }
          else{
            server.send(200, "text/plane", "0");
          }   
     });
     this->mcb= function;
}
void MailCubeWebServer::registerSwitchDeviceStateCallback(switchDeviceStateFunction function){
    sdsf=function;
    server.on("/removeNotification",HTTP_GET, [this](){
        sdsf(false);
        _syslog->addMessage("Removed Notification");
        server.send(200, "text/plane", "1");
    });
}
String MailCubeWebServer::getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  else if(filename.endsWith(".manifest")) return "text/cache-manifest";
  else if(filename.endsWith(".json")) return "text/json";
  return "text/plain";
}
bool MailCubeWebServer::handleFileRead(String path){
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  Serial.println("A client requested the follwing file: "+path);
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

String MailCubeWebServer::hardwareInfo(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  char stopic[100];
  int freeMem = ESP.getFreeHeap();
  String info;

  root["Ip Address"]=WiFi.localIP().toString();
  root["Subnet Mask"]=WiFi.subnetMask().toString();
  root["Gateway"]=WiFi.gatewayIP().toString();
  root["DNS Server"]=WiFi.dnsIP().toString();
  root["Wifi SSID"]=WiFi.SSID();
  root["Mac Address"]=WiFi.macAddress();
  root["Boot Counter"]=_settings->getBootCounter();
  root["Uptime"]=_syslog->getUptime();
  root["ESP Chip Id"]=String(ESP.getChipId());
  snprintf_P(stopic, sizeof(stopic), PSTR("0x%06X"), ESP.getFlashChipId());  
  root["Flash Chip ID"]=stopic;
  root["Flash Size"]=String(ESP.getFlashChipRealSize() / 1024)+"kB";
  root["Program Flash Size"]=String(ESP.getFlashChipSize() / 1024)+"kB";
  root["Program Size "]=String(ESP.getSketchSize() / 1024)+"kB";
  root["Free Program Space "]=String(ESP.getFreeSketchSpace() / 1024)+"kB";
  root["Free Memory "]=(String(freeMem / 1024))+"kB";
  root.printTo(info);
  return info;
}
void MailCubeWebServer::handleClient(){
  server.handleClient();
}