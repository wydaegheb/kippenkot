#ifndef KK_DOOR_H
#define KK_DOOR_H

#include <Arduino.h>

//  DOOR SETTINGS
#define DOOR_UNKNOWN 0
#define DOOR_OPENING 1
#define DOOR_OPENED 2
#define DOOR_CLOSING 3
#define DOOR_CLOSED 4

#define ENABLE_ORANGE_WIRE 25
#define IN1_GREEN_WIRE 26
#define IN2_BROWN_WIRE 27

class Door
{
public:
    void init();

    void closeDoor();
    void closeDoor(long ms);

    void openDoor();
    void openDoor(long ms);

    void stopDoor();

    int getDoorStatus() const;

    void setDoorStatus(int doorStatus);

private:
    int doorStatus = DOOR_UNKNOWN;
};

#endif // KK_DOOR_H
