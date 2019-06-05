#include "NtpClientLib.h"

#ifndef _MY_NTP
#define _MY_NTP

class MyNTP{
    

    private:
            int8_t _timeZone;
            int8_t _minutesTimeZone;
            boolean _syncEventTriggered; // True if a time even has been triggered
            NTPSyncEvent_t _ntpEvent; // Last triggered event
            String processSyncEvent (NTPSyncEvent_t _ntpEvent);
    public:
            MyNTP():_syncEventTriggered(false){};
            void initNtp (int8_t timeZone, int8_t minutesTimeZone);
            String getTime ();
            String getUptime ();
            String tick();
};
#endif