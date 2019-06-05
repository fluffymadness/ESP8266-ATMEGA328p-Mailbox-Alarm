#include "MyNTP.h"

void MyNTP::initNtp(int8_t timeZone, int8_t minutesTimeZone){
    _timeZone=timeZone;
    _minutesTimeZone=minutesTimeZone;
    NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
    NTP.setInterval (63);
    NTP.onNTPSyncEvent ([this](NTPSyncEvent_t event) {
        _ntpEvent = event;
        _syncEventTriggered = true;
    });
}
String MyNTP::getTime (){
    return(NTP.getTimeDateString()); 
}
String MyNTP::getUptime (){
    return(NTP.getUptimeString());
}

String MyNTP::processSyncEvent (NTPSyncEvent_t _ntpEvent) {
    if (_ntpEvent) {
        if (_ntpEvent == noResponse){
            return("Time Sync error: NTP server not reachable");
        }
        else if (_ntpEvent == invalidAddress){
            return("Time Sync error: Invalid NTP server address");
        }
            
    } else {
        return("Got NTP time: "+NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

String MyNTP::tick(){

    static int last=0;

    if (_syncEventTriggered) {
        String result=processSyncEvent (_ntpEvent);
        _syncEventTriggered = false;
        return (result);
    }
    return("OK");   
}

