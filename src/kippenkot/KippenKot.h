#ifndef KK_KIPPENKOT_H
#define KK_KIPPENKOT_H

#include <bluetooth/MyBluetooth.h>
#include <output/Printer.h>
#include <door/Door.h>

#include "proxies/Ina219Proxy.h"
#include "proxies/LightMeterProxy.h"
#include "proxies/ClockProxy.h"

#define LIGHT_MEASUREMENT_INTERVAL 5000
#define NIGHT_LUX_VALUE 100
#define MIN_NR_OF_CONSISTENT_LUX_READINGS 3

#define PRD false

struct IntResult
{
    char *remainingStr;
    int intValue;
};

class KippenKot
{
public:
    void init(MyBluetooth *myBluetooth, Ina219Proxy *ina219, LightMeterProxy *lightMeter, ClockProxy *myclock, Door *door, Printer *printer);

    void run();

    int getBeginDayHour() const;

    void setBeginDayHour(int beginDayHour);

    int getBeginNightHour() const;

    void setBeginNightHour(int beginNightHour);

    float getMaxCurrent() const;

    void setMaxCurrent(float maxCurrent);

    boolean getAutomaticMode() const;

    void setAutomaticMode(boolean automaticMode);

private:
    void handleBleCommands();
    bool stopBasedOnCurrent();
    void openDoorCompletely();
    void closeDoorCompletely(int tryCount);
    bool resolveNightOrDay();
    IntResult parseInt(char *str, char delim);

    MyBluetooth *_myBluetooth;
    Door *_door;
    Printer *_printer;
    Ina219Proxy *_ina219;
    LightMeterProxy *_lightMeter;
    ClockProxy *_myclock;

    int maxCurrent = 200;

    long MAX_TURN_TIME = 7500; // 5 rpm = 1 rotation / 12 sec = 1/2 rotation in 6 seconds + some margin
    boolean stopReceived = false;

    int nrOfConsistentLuxReadings = 0;
    bool isNightBasedOnLightMeter = false;
    bool isNightBasedOnClock = false;
    bool isNight = false;
    int beginDayHour = 8;
    int beginNightHour = 23;

    boolean automaticMode = true;
    boolean lightSensorEnabled = false;
    unsigned long _time;
};

#endif // KK_KIPPENKOT_H
