#pragma once

// Includes
#include <Arduino.h>
#include "include/I2C.h"
#include "include/PowerMeasurement.h"
#include "include/LedDriver.h"
#include "include/Network.h"
#include "include/Information.h"
#include "include/PirReader.h"

//-------------------- Basic Information --------------------//
#define Name         "12V LED Controller Mk4"
#define Programmer   "Nico Weidenfeller"
#define Created      "28.06.2020"
#define LastModifed  "04.08.2020"
#define Version      "0.17.0"

/*
      ToDo     Fix bug with power measurements reading zero from reg
      ToDo     Fix LED on when bootup
      ToDo     Add NTP Summer/Winter time swap => For now only Winter Time 
      ToDo     Rework doc string. Information about how to use stuff in header, and how it works in source 
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
      unsigned long PrevMillis_Loop = 0;

   public:
      I2C i2c = I2C();
      Network network = Network();
      PowerMeasurement powerMessurement = PowerMeasurement(INA219AIDR_I2C_ADDRESS, &i2c, &network);
      PirReader pirReader = PirReader(PIR_SENSOR_1_PIN, PIR_SENSOR_2_PIN, &network);
      LedDriver ledDriver = LedDriver(PCA9685PW_I2C_ADDRESS, &i2c, &network, &pirReader);
      Information information = Information(&network, &pirReader);

   // ## Functions ## //
   private:
   public:
      void _loop();
      void _setup();

};

// Tell compiler to create only one instance of Main
extern Main mainController;