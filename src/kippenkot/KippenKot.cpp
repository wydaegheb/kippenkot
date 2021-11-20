//
// Created by admin on 19/11/2021.
//

#include <RTClib.h>
#include "KippenKot.h"

void KippenKot::init(MyBluetooth *myBluetooth, Adafruit_INA219 *ina219, BH1750 *lightMeter, RTC_DS3231 *myclock, Door *door, Printer *printer) {
    _myBluetooth = myBluetooth;
    _ina219 = ina219;
    _lightMeter = lightMeter;
    _myclock = myclock;
    _door = door;
    _printer = printer;

    Wire.begin(); // IMPORTANT!!! Without it the lightmeter does not start in "disconnected mode" (no USB connected to arduino) !!!!

    if (!PRD) { // do not initialiase Serial in PRD (no USB cable connected) as it hangs the esp
        Serial.begin(115200);
    }

    // Initialise bleutooth
    _printer->print("\n\n!!!The Ultimate Kippenkot!!!!\n\n");
    _printer->print("Initialising bleutooth");
    _myBluetooth->init();
    _printer->print(" - bleutooth initialised\n");

    // Initialise printer
    _printer->init(_myBluetooth, PRD);

    // Initialize the INA219.
    _printer->print("Initialising the current sensor");
    if (!_ina219->begin()) {
        _printer->error(" - FAILED. Failed to find INA219 chip");
    }
    _printer->print(" - current sensor INA 219 initialised\n");

    // Initialise door
    _printer->print("Initialising the door motor\n ");
    _door->init();
    _time = millis();
    _printer->print(" - door status is unknown -> ");
    openDoorCompletely();
    _printer->print(" - door motor initialised\n");

    // intialise light sensor
    _printer->print("Initialising lightmeter BH1750.");
    if (!_lightMeter->begin()) {
        _printer->error(" - FAILED to initialise light meter!");
    }
    _printer->print(" - lightmeter BH1750 initialised\n");

    // initialise clock
    _printer->print("Initialising clock.");
    if (!_myclock->begin()) {
        _printer->error(" - FAILED to initialise clock!");
    }
    if (_myclock->lostPower()) {
        _printer->print("\n - clock lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        _myclock->adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    _printer->print(" - clock initialised\n\n");

    _printer->print("The ultimate Kippenkot is ready for action!\n");
}

void KippenKot::run() {
    if ((millis() - _time) > LIGHT_MEASUREMENT_INTERVAL) {
        if (automaticMode) {
            _printer->print("\n");
            boolean isNight = resolveNightOrDay();
            if (isNight && (_door->getDoorStatus() == DOOR_OPENED)) {
                closeDoorCompletely(0);
            }
            if (!isNight && (_door->getDoorStatus() == DOOR_CLOSED)) {
                openDoorCompletely();
            }
        } else {
            _printer->print("\nRunning in MANUAL mode.\nUse the app to open or close the door.\n");
        }
        _time = millis();
    }
    handleBleCommands();
}

void KippenKot::handleBleCommands() {
    char c = (char) _myBluetooth->getLastCommand();
    if (c == STATUS_BL_COMMAND) { // status
        _printer->print("\nKIPPENKOT STATUS\n================================\n");

        DateTime now = _myclock->now();
        char buffer[32]; //make this big enough to hold the resulting string
        snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d", now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
        _printer->print(" - TIME:   ", buffer, "\n");
        _printer->print(" - DOOR POWER:   ", abs(_ina219->getCurrent_mA()), " mA\n");
        _printer->print(" - DAY STARTS AT:  ", beginDayHour, " hour (24h format)\n");
        _printer->print(" - NIGHT STARTS AT:  ", beginNightHour, " hour (24h format)\n");
        _printer->print(" - DOOR STATUS:  ");
        if (_door->getDoorStatus() == DOOR_OPENING) {
            _printer->print("OPENING\n");
        } else if (_door->getDoorStatus() == DOOR_OPENED) {
            _printer->print("OPEN\n");
        } else if (_door->getDoorStatus() == DOOR_CLOSING) {
            _printer->print("CLOSING\n");
        } else if (_door->getDoorStatus() == DOOR_CLOSED) {
            _printer->print("CLOSED\n");
        } else {
            _printer->print("UNKNOWN\n");
        }
        _printer->print(" - MODE:  ");
        if (automaticMode) {
            _printer->print("Automatic\n");
        } else {
            _printer->print("!!MANUAL!!\n(door will not open or close automatically)\n");
        }
        _printer->print("================================\n");

    } else if (c == DATE_AND_TIME_BL_COMMAND) { // date and time
        delay(1000); // wait for next message to arrive
        char *timeStr = _myBluetooth->getLastMessage();
        IntResult yearRes = parseInt(timeStr, '/');
        int year = yearRes.intValue;
        IntResult monthRes = parseInt(yearRes.remainingStr, '/');
        int month = monthRes.intValue + 1;
        IntResult dayRes = parseInt(monthRes.remainingStr, ' ');
        int day = dayRes.intValue;
        IntResult hourRes = parseInt(dayRes.remainingStr, ':');
        int hour = hourRes.intValue;
        IntResult minRes = parseInt(hourRes.remainingStr, '#');
        int minutes = minRes.intValue;
        _myclock->adjust(DateTime(year, month, day, hour, minutes, 0));
        std::stringstream os;
        os << "\nSetting date and time to:" << year << "/" << month << "/" << day << " " << hour << ":" << minutes << "\nSucces. Date and time updated.\n";
        _printer->print(os.str());

    } else if (c == START_DAY_HOUR_BLE_COMMAND) { // day start
        delay(1000); // wait for next message to arrive
        char *startDayHourStr = _myBluetooth->getLastMessage();
        IntResult beginDayHourRes = parseInt(startDayHourStr, '#');
        beginDayHour = beginDayHourRes.intValue;
        _printer->print("\nSucces. Day start hour updated to: ", beginDayHour, "\n");
    } else if (c == START_NIGHT_HOUR_BLE_COMMAND) { // night start
        delay(1000); // wait for next message to arrive
        char *startNightHourStr = _myBluetooth->getLastMessage();
        IntResult beginNightHourRes = parseInt(startNightHourStr, '#');
        beginNightHour = beginNightHourRes.intValue;
        _printer->print("\nSucces. Night start hour updated to: ", beginNightHour, "\n");
    } else if (c == CRUSHED_CHICKEN_CURRENT_COMMAND) { // cut of power crushed chicken
        delay(1000); // wait for next message to arrive
        char *maxCurrentStr = _myBluetooth->getLastMessage();
        IntResult maxCurrentRes = parseInt(maxCurrentStr, '#');
        maxCurrent = maxCurrentRes.intValue;
        _printer->print("\nSucces. Crushed chicken power updated to: ", maxCurrent, "\n");
    } else if (c == CLOSE_BL_COMMAND) { // close
        _printer->print("\nBL: Closing door -> MANUAL MODE!!!!\n");
        automaticMode = false;
        closeDoorCompletely(0);
    } else if (c == OPEN_BL_COMMAND) { // open
        _printer->print("\nBL: Opening door -> MANUAL MODE!!!!\n");
        automaticMode = false;
        openDoorCompletely();
    } else if (c == STOP_BL_COMMAND) { // stop
        _printer->print("\nBL: Stop door -> MANUAL MODE!!!!\n");
        automaticMode = false;
        stopReceived = true;
        _door->stopDoor();
    } else if (c == AUTOMATIC_ENABLED_COMMAND) { // re-enable automatic handling
        _printer->print("\nAutomatic mode enabled again\n");
        automaticMode = true;
    }
}

bool KippenKot::stopBasedOnCurrent() {
    float current_mA = abs(_ina219->getCurrent_mA());
    if (current_mA > maxCurrent) {
        _printer->print("Current:       ", current_mA, " mA (retrying)\n");
        delay(100);
        current_mA = abs(_ina219->getCurrent_mA());
        if (current_mA > maxCurrent) {
            _printer->print("Current:       ", current_mA, " mA. Stopping current above treshold!\n");
            return true;
        }
    }
    return false;
}

void KippenKot::openDoorCompletely() {
    unsigned long stopTime = millis() + MAX_TURN_TIME;

    _door->setDoorStatus(DOOR_OPENING);
    _printer->print("Opening door for maximum: ", MAX_TURN_TIME, " ms or limit power triggered.\n");

    _door->openDoor();

    stopReceived = false;
    while (millis() < stopTime && !stopReceived) {
        delay(1);
        stopReceived = _myBluetooth->receivedBleStop() || stopBasedOnCurrent();
    }
    _printer->print("Door opened or stopped.\n");
    _door->closeDoor(100);
    _door->setDoorStatus(DOOR_OPENED);
}

void KippenKot::closeDoorCompletely(int tryCount) {
    tryCount++;
    unsigned long startTime = millis();
    unsigned long stopTime = startTime + MAX_TURN_TIME;

    _door->setDoorStatus(DOOR_CLOSING);
    _printer->print("Closing door for maximum: ", MAX_TURN_TIME, " ms or limit power triggered.\n");

    _door->closeDoor();

    stopReceived = false;
    boolean stopMaxCurrent = false;
    boolean stopCrushedChicken = false;
    while (millis() < stopTime && !stopReceived && !stopMaxCurrent) {
        delay(1);
        stopReceived = _myBluetooth->receivedBleStop();
        stopMaxCurrent = stopBasedOnCurrent();
        if (millis() < (startTime + 5500) && stopMaxCurrent) { // always stop in the first 5.5 sec afterwards assume its the door not a chicken
            stopCrushedChicken = true;
            _printer->print("Crushed chicken detected!\n");
        }
    }
    _door->stopDoor();
    if (!stopCrushedChicken) {
        _printer->print("Door closed or stopped.\n");
        _door->openDoor(100); // just a little debounce
        _door->setDoorStatus(DOOR_CLOSED);
    } else {
        if (tryCount < 3) {
            _printer->print("Crushed chicken detected - opening door again.\n");
            openDoorCompletely();
            delay(1000);
            _printer->print("Trying to close door again.\n");
            closeDoorCompletely(tryCount);
        } else {
            _printer->print("Tried to many times to close the door - giving up.");
            _door->openDoor(100); // just a little debounce
            _door->setDoorStatus(DOOR_UNKNOWN);
        }
    }
}

bool KippenKot::resolveNightOrDay() {
    bool isNight;

    // measure light
    float lux = _lightMeter->readLightLevel();
    boolean measuredIsNightValue = (lux < NIGHT_LUX_VALUE);
    if (isNightBasedOnLightMeter != measuredIsNightValue) {
        nrOfConsistentLuxReadings++;
    } else {
        nrOfConsistentLuxReadings = 1;
    }

    if (nrOfConsistentLuxReadings > MIN_NR_OF_CONSISTENT_LUX_READINGS) {
        nrOfConsistentLuxReadings = 1;
        isNightBasedOnLightMeter = measuredIsNightValue;
    }

    if (measuredIsNightValue) {
        _printer->print("Light: ", lux, " lux (=night) - count: ", nrOfConsistentLuxReadings, "/3\n");
    } else {
        _printer->print("Light: ", lux, " lux (=day) - count: ", nrOfConsistentLuxReadings, "/3\n");
    }

    // get time
    DateTime now = _myclock->now();
    int hour = now.hour();
    char buffer[32]; //make this big enough to hold the resulting string
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%4d %02d:%02d:%02d", now.day(), now.month(), now.year(), now.hour(),
             now.minute(), now.second());
    if ((hour >= beginNightHour) || (hour < beginDayHour)) {
        _printer->print("Clock: ", buffer, " (=night) - ", _myclock->getTemperature(), " C\n");
        isNightBasedOnClock = true;
    } else {
        _printer->print("Clock: ", buffer, " (=day) - ", _myclock->getTemperature(), " C\n");
        isNightBasedOnClock = false;
    }

    // resolve
    if (isNightBasedOnClock) { // clock overrides night time (even when there is light in the night the door wil not open)
        isNight = true;
    } else { // during "day time hours" the light sensor determines the state of the door (after a configurable number of consistent light readings)
        isNight = isNightBasedOnLightMeter;
    }
    if (isNight) {
        _printer->print("-> Night busy\n");
    } else {
        _printer->print("-> Day busy\n");
    }
    return isNight;
}

int KippenKot::getBeginDayHour() const {
    return beginDayHour;
}

void KippenKot::setBeginDayHour(int beginDayHour) {
    KippenKot::beginDayHour = beginDayHour;
}

int KippenKot::getBeginNightHour() const {
    return beginNightHour;
}

void KippenKot::setBeginNightHour(int beginNightHour) {
    KippenKot::beginNightHour = beginNightHour;
}

float KippenKot::getMaxCurrent() const {
    return maxCurrent;
}

void KippenKot::setMaxCurrent(float maxCurrent) {
    KippenKot::maxCurrent = maxCurrent;
}

boolean KippenKot::getAutomaticMode() const {
    return automaticMode;
}

void KippenKot::setAutomaticMode(boolean automaticMode) {
    KippenKot::automaticMode = automaticMode;
}

IntResult KippenKot::parseInt(char *str, char delim) {
    int i = 0;
    char buffer[10];
    while (*str != delim) {
        buffer[i] = *str;
        i++;
        str++;
    }

    buffer[i] = '\0';
    Serial.println(buffer);
    IntResult result;
    result.remainingStr = ++str;
    result.intValue = atoi(buffer);
    return result;
}







