#include "Log.h"

void Log::addMessage(String text) {
    if(*_ntp==nullptr){
        log_messages.push_back(text);
        Serial.println(text);
    }
    else{
        log_messages.push_back((*_ntp)->getTime()+":"+text);
        Serial.println((*_ntp)->getTime()+":"+text);
    }
    if(log_messages.size()==log_size){
        log_messages.pop_front();
    }
    
}
String Log::toString(){
    String temp;
    for(String n:log_messages){
        temp+=n;
        temp+="\n";
    }
    return temp;
}
String Log::getUptime(){
    if(*_ntp==nullptr){
        return "";
    }
    return (*_ntp)->getUptime();
}



