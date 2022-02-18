#ifndef KK_MYBLUETOOTH_H
#define KK_MYBLUETOOTH_H

#include "Arduino.h"

#include <utility>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <mutex>

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define EMPTY_COMMAND ' '

#define STOP_BL_COMMAND '1'
#define OPEN_BL_COMMAND '2'
#define CLOSE_BL_COMMAND '3'
#define STATUS_BL_COMMAND '4'
#define DATE_AND_TIME_BL_COMMAND '5'
#define START_DAY_HOUR_BLE_COMMAND '6'
#define START_NIGHT_HOUR_BLE_COMMAND '7'
#define CRUSHED_CHICKEN_CURRENT_COMMAND '8'
#define AUTOMATIC_ENABLED_COMMAND '9'
#define TOGGLE_BLE_LOGGING '0'
#define IGNORE_LIGHT_SENSOR 'A'
#define USE_LIGHT_SENSOR 'B'

class MyBluetooth : public BLEServerCallbacks, BLECharacteristicCallbacks
{
public:
    void init();

    void print(std::string s);

    void onWrite(BLECharacteristic *pCharacteristic) override;

    void onConnect(BLEServer *pServer) override;

    void onDisconnect(BLEServer *pServer) override;

    bool receivedBleStop();

    char getLastCommand();

    char *getLastMessage();

    bool isDeviceConnected() const;

private:
    BLEServer *pServer = nullptr;
    BLECharacteristic *pTxCharacteristic;
    bool deviceConnected = false;

    char _lastCommand = EMPTY_COMMAND;
    std::string _lastMessage = "";
    std::mutex mtx; // mutex for critical section
};

#endif // KK_MYBLUETOOTH_H
