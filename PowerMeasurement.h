#ifndef POWER_MESSURE_H_INCLUDE
#define POWER_MESSURE_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "Network.h"

namespace LedControllerSoftwareMk5
{

    class PowerMeasurement
    {
        // ## Data ## //
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        Network *network;
        bool init = false;
        // Timer
        unsigned long PrevMillis_PowerMessurmentUpdateRate = 0;
        const unsigned long TimeOut_PowerMessurmentUpdateRate = 1000; // 1 sec
        // Register
        const uint8_t regConfig = 0x00;
        const uint8_t regShuntVolt = 0x01;
        const uint8_t regBusVolt = 0x02;
        const uint8_t regPower = 0x03;
        const uint8_t regCurrent = 0x04;
        const uint8_t regCalibration = 0x05;
        // Values
        double valueShunt_mV = 0.0;
        double valueBus_V = 0.0;
        double valuePower_mW = 0.0;
        double valueCurrent_mA = 0.0;
    
    public:

    // ## Functions ## //
    private:

    public:
        PowerMeasurement(uint8_t i2cAddress, I2C *i2c, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif