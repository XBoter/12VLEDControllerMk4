#pragma once

#include "Webserver/Webserver.h"
#include "Enums/Enums.h"
#include "I2C/I2C.h"
#include "Information/Information.h"
#include "Interface/IBaseClass.h"
#include "LedDriver/LedDriver.h"
#include "Network/Network.h"
#include "OTA/OTA.h"
#include "PirReader/PirReader.h"
#include "PowerMeasurement/PowerMeasurement.h"
#include "Filesystem/Filesystem.h"
#include "Parameterhandler/Parameterhandler.h"
#include "Helper/Helper.h"
#include "Register/INA219AIDR_Reg.h"
#include "Register/PCA9685_LED_Reg.h"
#include "Structs/Structs.h"

// ================ Basic Information ================ //
#define Name "12V LED Controller Mk4"
#define Programmer "Nico Weidenfeller"
#define Created "28.06.2020"
#define LastModifed "23.12.2021"
#define Version "2.0.0"

// ================ Global Defines ================ //
#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45
#define PIR_SENSOR_1_PIN D6
#define PIR_SENSOR_2_PIN D7

#define cycle 10

class LEDControllerMk4
{

public:
    LEDControllerMk4();
    void _setup();
    void _loop();

private:
    uint8_t state = 0;

    // ================ Components ================ //
    I2C i2c = I2C();
    Webserver webserver = Webserver();
    Helper helper = Helper();
    Filesystem filesystem = Filesystem();
    Network network = Network(Version);
    OTA ota = OTA();
    PowerMeasurement powerMessurement = PowerMeasurement(INA219AIDR_I2C_ADDRESS,
                                                         0.002); // 2 mOhm
    PirReader pirReader = PirReader(PIR_SENSOR_1_PIN,
                                    PIR_SENSOR_2_PIN);
    LedDriver ledDriver = LedDriver(PCA9685PW_I2C_ADDRESS);
    Information information = Information();
    Parameterhandler parameterhandler = Parameterhandler();

    // ================ Performance Calculation ================ //
    bool enablePerformanceMonitor = false;

    unsigned long difTimeI2C[cycle];
    unsigned long difTimeWebserver[cycle];
    unsigned long difTimeNetwork[cycle];
    unsigned long difTimeOTA[cycle];
    unsigned long difTimePowerMeasurement[cycle];
    unsigned long difTimePIR[cycle];
    unsigned long difTimeLED[cycle];
    unsigned long difTimeInformation[cycle];
    unsigned long difTimeHelper[cycle];
    unsigned long difTimeFilesystem[cycle];
    unsigned long difTimeParameterhandler[cycle];
    unsigned long difTimeAll[cycle];

    uint16_t localCycle = 10;
    uint16_t cycleCounter = 0;
    void CalcPerformance();
};
