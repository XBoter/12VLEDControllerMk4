#pragma once

// Includes
#include <Arduino.h>
#include "I2C.h"
#include "Network.h"
#include "INA219AIDR_Reg.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class PowerMeasurement : public IBaseClass
{
    // ## Constructor ## //
    public:
         PowerMeasurement(uint8_t i2cAddress, 
                          I2C *i2c, 
                          Network *network);

    // ## Interface ## //
    private:
    public:
        virtual bool Init();
        virtual void Run();

    // ## Data ## //
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        Network *network;
        // Timer
        unsigned long PrevMillis_PowerMessurmentUpdateRate = 0;
        const unsigned long TimeOut_PowerMessurmentUpdateRate = 1000; // 1 sec
        // Values
        double valueShunt_mV = 0.0;
        double valueBus_V = 0.0;
        double valuePower_mW = 0.0;
        double valueCurrent_mA = 0.0;

    public:

    // ## Functions ## //
    private:
        void PrintAllRegister();
        void Print2ByteValue(uint16_t data);
    public:

};
