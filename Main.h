// Include guard
#ifndef MAIN_H_INCLUDE
#define MAIN_H_INCLUDE


#include "Arduino.h"
#include "I2C.h"
#include "PowerMeasurement.h"
#include "LedDriver.h"
#include "Network.h"
#include "Information.h"
#include "PirReader.h"

//-------------------- Basic Information --------------------//
#define Name        "12V LED Controller Mk4"
#define Programmer  "Nico Weidenfeller"
#define Created     "28.06.2020"
#define LastModifed "31.07.2020"
#define Version     "0.15.0"

/*
      ToDo     Fix bug with power measurements reading zero from reg
      ToDo     Fix LED on when bootup
      ToDo     Mqtt Reconnect Color Update etc
*/

//++++ Global Defines ++++//

#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45 
#define PIR_SENSOR_1_PIN D6
#define PIR_SENSOR_2_PIN D7


class Main
{
    // ## Data ## //
    private:

    public:
       LedControllerSoftwareMk5::I2C i2c = LedControllerSoftwareMk5::I2C();
       LedControllerSoftwareMk5::Network network = LedControllerSoftwareMk5::Network();
       LedControllerSoftwareMk5::PowerMeasurement powerMessurement = LedControllerSoftwareMk5::PowerMeasurement(INA219AIDR_I2C_ADDRESS, &i2c, &network);
       LedControllerSoftwareMk5::PirReader pirReader = LedControllerSoftwareMk5::PirReader(PIR_SENSOR_1_PIN, PIR_SENSOR_2_PIN, &network);
       LedControllerSoftwareMk5::LedDriver ledDriver = LedControllerSoftwareMk5::LedDriver(PCA9685PW_I2C_ADDRESS, &i2c, &network, &pirReader);
       LedControllerSoftwareMk5::Information information = LedControllerSoftwareMk5::Information(&network, &pirReader);

    // ## Functions ## //
    private:

    public:
       Main();
       void _loop();
       void _setup();
};

extern Main mainController;

#endif