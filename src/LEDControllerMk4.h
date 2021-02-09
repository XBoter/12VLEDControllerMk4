#pragma once

#include "Configuration/Configuration.h"
#include "Enums/Enums.h"
#include "I2C/I2C.h"
#include "Information/Information.h"
#include "Interface/IBaseClass.h"
#include "LedDriver/LedDriver.h"
#include "Network/Network.h"
#include "OTA/OTA.h"
#include "PirReader/PirReader.h"
#include "PowerMeasurement/PowerMeasurement.h"
#include "Register/INA219AIDR_Reg.h"
#include "Register/PCA9685_LED_Reg.h"
#include "Structs/Structs.h"

//-------------------- Basic Information --------------------//
#define Name "12V LED Controller Mk4"
#define Programmer "Nico Weidenfeller"
#define Created "28.06.2020"
#define LastModifed "05.02.2021"
#define Version "1.2.4"

//++++ Global Defines ++++//
#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45
#define PIR_SENSOR_1_PIN D6
#define PIR_SENSOR_2_PIN D7

class LEDControllerMk4
{

public:
    LEDControllerMk4();
    void _setup();
    void _loop();

private:
    // ## Data ## //
    unsigned long PrevMicros_Loop = 0;
    uint8_t state = 0;

    // ## Components ## //
    I2C i2c = I2C();
    Configuration configuration = Configuration();
    Network network = Network();
    Network memNetwork = Network(); // Memory Version Instance for Information
    OTA ota = OTA();
    PowerMeasurement powerMessurement = PowerMeasurement(INA219AIDR_I2C_ADDRESS,
                                                         0.002); // 2 mOhm
    PirReader pirReader = PirReader(PIR_SENSOR_1_PIN,
                                    PIR_SENSOR_2_PIN);
    PirReader memPirReader = PirReader(0, 0); // Memory Version Instance for Information
    LedDriver ledDriver = LedDriver(PCA9685PW_I2C_ADDRESS);
    Information information = Information();
};
