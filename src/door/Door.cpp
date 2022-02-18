#include "Door.h"

void Door::closeDoor()
{
    digitalWrite(IN1_GREEN_WIRE, HIGH);    // set the spinning direction clockwise
    digitalWrite(IN2_BROWN_WIRE, LOW);     // set the spinning direction clockwise
    digitalWrite(ENABLE_ORANGE_WIRE, 255); // enable motor
}

void Door::closeDoor(long ms)
{
    stopDoor();
    closeDoor();
    delay(ms);
    stopDoor();
}

void Door::openDoor()
{
    digitalWrite(IN1_GREEN_WIRE, LOW);     // set the spinning direction counterclockwise
    digitalWrite(IN2_BROWN_WIRE, HIGH);    // set the spinning direction counterclockwise
    digitalWrite(ENABLE_ORANGE_WIRE, 255); // enable motor
}

void Door::openDoor(long ms)
{
    stopDoor();
    openDoor();
    delay(ms);
    stopDoor();
}

void Door::stopDoor()
{
    digitalWrite(ENABLE_ORANGE_WIRE, 0); // disable motor
    digitalWrite(IN1_GREEN_WIRE, LOW);   // stop spinning
    digitalWrite(IN2_BROWN_WIRE, LOW);   // stop spinning
}

void Door::init()
{
    pinMode(ENABLE_ORANGE_WIRE, OUTPUT);
    pinMode(IN1_GREEN_WIRE, OUTPUT);
    pinMode(IN2_BROWN_WIRE, OUTPUT);
    stopDoor();
}

int Door::getDoorStatus() const
{
    return doorStatus;
}

void Door::setDoorStatus(int doorStatus)
{
    Door::doorStatus = doorStatus;
}
