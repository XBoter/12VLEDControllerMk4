#pragma once

// Includes
#include <Arduino.h>
#include "include/I2C.h"
#include "include/PowerMeasurement.h"
#include "include/LedDriver.h"
#include "include/Network.h"
#include "include/Information.h"
#include "include/PirReader.h"
#include "include/Configuration.h"
#include "include/OTA.h"

//-------------------- Basic Information --------------------//
#define Name "12V LED Controller Mk4"
#define Programmer "Nico Weidenfeller"
#define Created "28.06.2020"
#define LastModifed "05.02.2021"
#define Version "1.2.3"

/*
      ToDo     Change Shut Resistor to 0.02 Ohm for better reading 
      ToDo     Add NTP Summer/Winter time swap => For now only Winter Time 
      ToDo     Add PCA9685 reset after to many I2C errorss
      ToDo     Add option to sync diffrent single strip led effects
      ToDo     Add ESP OTA GitHub upgrade 
      ToDo     Update Function description
      ToDo     Maybe add option for led strip pin map order
*/

//++++ Global Defines ++++//
#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45
#define PIR_SENSOR_1_PIN D6
#define PIR_SENSOR_2_PIN D7

// Classes
class Main
{
   // ## Constructor ## //
public:
   Main();

   // ## Data ## //
private:
   unsigned long PrevMicros_Loop = 0;
   uint8_t state = 0;

public:
   // Components
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

   // ## Functions ## //
private:
public:
   void _loop();
   void _setup();
};

// Tell compiler to create only one instance of Main
extern Main mainController;
