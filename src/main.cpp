#include <kippenkot/KippenKot.h>

// Door
Door door;

// Bluetooth
MyBluetooth myBluetooth;

// Printer
Printer printer;

// Current measure
Adafruit_INA219 ina219;

//  Light meter
BH1750 lightMeter;

//  Clock
RTC_DS3231 myclock;

// KippenKot
KippenKot kippenKot;



void setup()
{

    kippenKot.init(&myBluetooth, &ina219, &lightMeter, &myclock, &door, &printer);
}

void loop()
{
    kippenKot.run();
}
