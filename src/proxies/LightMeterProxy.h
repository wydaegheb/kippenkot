#ifndef KK_LIGHTMETER_MOCK_H
#define KK_LIGHTMETER_MOCK_H

#include <Arduino.h>
#include <BH1750.h>

class LightMeterProxy
{
public:
    LightMeterProxy(BH1750 *bh1750, bool PRD)
    {
        _bh1750 = bh1750;
        _prd = PRD;
    }

    boolean begin()
    {
        if (_prd)
        {
            return _bh1750->begin();
        }
        else
        {
            return true;
        }
    }

    float readLightLevel()
    {
        if (_prd)
        {
            return _bh1750->readLightLevel();
        }
        else
        {
            return 0.0;
        }
    }

private:
    BH1750 *_bh1750;
    bool _prd;
};

#endif