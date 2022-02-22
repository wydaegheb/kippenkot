#include <iomanip>
#include "Printer.h"

void Printer::init(MyBluetooth *myBluetooth, bool printToSerial)
{
    print("Initializing printer");

    _myBluetooth = myBluetooth;
    _printToSerial = printToSerial;

    print(" - printer initialized.\n");
}

void Printer::print(const char *s)
{
    std::stringstream outstring;
    outstring << s;
    print(outstring.str());
}

void Printer::print(const char *s, long l, const char *s2)
{
    std::stringstream outstring;
    outstring << s << l << s2;
    print(outstring.str());
}

void Printer::print(const char *s, float f, const char *s2)
{
    std::stringstream outstring;
    outstring << s << std::fixed << std::setprecision(1) << f << s2;
    print(outstring.str());
}

void Printer::print(const char *s1, int i, const char *s2)
{
    std::stringstream outstring;
    outstring << s1 << i << s2;
    print(outstring.str());
}

void Printer::print(const char *s1, const char *s2, const char *s3)
{
    std::stringstream outstring;
    outstring << s1 << s2 << s3;
    print(outstring.str());
}

void Printer::print(const char *s, float f, const char *s2, int i, const char *s3)
{
    std::stringstream outstring;
    outstring << s << std::fixed << std::setprecision(1) << f << s2 << i << s3;
    print(outstring.str());
}

void Printer::print(const char *s1, const char *s2, const char *s3, float f, const char *s4)
{
    std::stringstream outstring;
    outstring << s1 << s2 << s3 << std::fixed << std::setprecision(1) << f << s4;
    print(outstring.str());
}

void Printer::print(const std::string &s)
{
    if (_printToSerial)
    {
        Serial.print(F(s.c_str()));
    }
    if (_myBluetooth != nullptr && _myBluetooth->isDeviceConnected())
    {
        _myBluetooth->print(s);
    }
}

void Printer::error(const std::string &err)
{
    print(err.c_str());
    while (1);
}
