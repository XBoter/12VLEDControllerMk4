#ifndef POWER_MESSURE_H_INCLUDE
#define POWER_MESSURE_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "Network.h"

namespace LedControllerSoftwareMk5
{

    class PowerMeasurement
    {
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        Network *network;
        bool init = false;
        unsigned long PrevMillis_PowerMessurmentUpdateRate = 0;
        const unsigned long TimeOut_PowerMessurmentUpdateRate = 500; // 0.5 sec
        // Register
        const uint8_t regConfig = 0x00;
        const uint8_t regShuntVolt = 0x01;
        const uint8_t regBusVolt = 0x02;
        const uint8_t regPower = 0x03;
        const uint8_t regCurrent = 0x04;
        const uint8_t regCalibration = 0x05;
        double currentDivider_mA;
        double powerMultiplier_mW;
        double calibrationValue;
        double correctedCalibrationValue;
        double currentLSB;
        void printDouble(double val, unsigned int precision);
        void calc();

    public:
        PowerMeasurement(uint8_t i2cAddress, I2C *i2c, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif