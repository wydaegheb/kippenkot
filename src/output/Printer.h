#ifndef KK_PRINTER_H
#define KK_PRINTER_H

#include <string>
#include <sstream>

#include <bluetooth/MyBluetooth.h>

class Printer
{
public:
    void init(MyBluetooth *myBluetooth, bool printToSerial);
    void print(const char *s);
    void print(const char *s1, long l, const char *s2);
    void print(const char *s1, float f, const char *s2);
    void print(const char *s1, int i, const char *s2);
    void print(const char *s1, const char *s2, const char *s3);
    void print(const char *s1, float f, const char *s2, int i, const char *s3);
    void print(const char *s1, const char *s2, const char *s3, float f, const char *s4);
    void print(const std::string &s);
    void error(const std::string &err);

private:
    MyBluetooth *_myBluetooth;
    bool _printToSerial;
};

#endif // KK_PRINTER_H
