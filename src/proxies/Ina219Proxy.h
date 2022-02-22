#ifndef KK_INA219MOCK_H
#define KK_INA219MOCK_H

#include <Arduino.h>
#include <Adafruit_INA219.h>

class Ina219Proxy
{
public:
    Ina219Proxy(Adafruit_INA219 *ina219, bool PRD)
    {
        _ina219 = ina219;
        _prd = PRD;
    }

    bool begin()
    {
        if (_prd)
        {
            return _ina219->begin();
        }
        else
        {
            return true;
        }
    }

    float getCurrent_mA()
    {
        if (_prd)
        {
            return _ina219->getCurrent_mA();
        }
        else
        {
            return 10.0;
        }
    }

private:
    Adafruit_INA219 *_ina219;
    bool _prd;
};

#endif
