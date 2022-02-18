#include "MyBluetooth.h"

void MyBluetooth::init()
{

    // Initialise bleutooth
    // Create the BLE Device
    BLEDevice::init("Karlijns Kiekenkot");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(this);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();

    _lastMessage = "";
}

void MyBluetooth::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
};

void MyBluetooth::onDisconnect(BLEServer *pServer)
{
    deviceConnected = false;
}

void MyBluetooth::onWrite(BLECharacteristic *pCharacteristic)
{
    mtx.lock();
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() == 1)
    {
        if (_lastCommand == EMPTY_COMMAND)
        {
            _lastCommand = rxValue[0];
            _lastMessage.clear();
        }
    }
    else if (rxValue.length() > 1)
    {
        if (_lastMessage.length() == 0)
        {
            _lastMessage = rxValue;
        }
    }
    mtx.unlock();
}

void MyBluetooth::print(std::string s)
{
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(std::move(s));
        pTxCharacteristic->notify();
    }
}

bool MyBluetooth::receivedBleStop()
{
    if (_lastCommand == STOP_BL_COMMAND)
    {
        getLastCommand();
        return true;
    }
    return false;
}

char MyBluetooth::getLastCommand()
{
    mtx.lock();
    char result = _lastCommand;
    _lastCommand = EMPTY_COMMAND;
    mtx.unlock();
    return result;
}

char *MyBluetooth::getLastMessage()
{
    mtx.lock();
    char *result = new char[_lastMessage.length() + 1];
    strcpy(result, _lastMessage.c_str());
    _lastMessage.clear();
    mtx.unlock();
    return result;
}

bool MyBluetooth::isDeviceConnected() const
{
    return deviceConnected;
}
