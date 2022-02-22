#ifndef KK_CLOCK_PROXY_H
#define KK_CLOCK_PROXY_H

#include <Arduino.h>
#include <RTClib.h>

class ClockProxy
{
public:
    ClockProxy(RTC_DS3231 *myClock, bool PRD)
    {
        _myClock = myClock;
        _prd = PRD;
    }

    boolean begin()
    {
        if (_prd)
        {
            return _myClock->begin();
        }
        else
        {
            _startTime = (uint32_t) millis();
            return true;
        }
    }

    boolean lostPower()
    {
        if (_prd)
        {
            return _myClock->lostPower();
        }
        else
        {
            return false;
        }
    }

    void adjust(DateTime dateTime)
    {
        if (_prd)
        {
            _myClock->adjust(dateTime);
        }
        else
        {
            _currentTime = dateTime.secondstime();
        }
    }

    DateTime now()
    {
        if (_prd)
        {
            return _myClock->now();
        }
        else
        {
            return DateTime(_currentTime + (millis()-_startTime));
        }
    }

    float getTemperature()
    {
        if (_prd)
        {
            return _myClock->getTemperature();
        }
        else
        {
            return 23.0;
        }
    }

private:
    RTC_DS3231 *_myClock;
    bool _prd;

    uint32_t _startTime = 0;
    uint32_t _currentTime = 0;
};

#endif