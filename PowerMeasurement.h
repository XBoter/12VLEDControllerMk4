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

    public:
        PowerMeasurement(uint8_t i2cAddress, I2C *i2c, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif