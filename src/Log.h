
#include <deque>
#include <Arduino.h>
#ifndef _LOG
#define _LOG
#define log_size 100
#include "MyNTP.h"

class Log{

    private:
        std::deque<String> log_messages;
        MyNTP** _ntp;


    public:
        Log(MyNTP** ntp):_ntp(ntp){}
        void addMessage(String message);
        String toString();
        String getUptime();


};
#endif

