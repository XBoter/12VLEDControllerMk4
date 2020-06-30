#ifndef LED_DRIVER_H_INCLUDE
#define LED_DRIVER_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "Network.h"

namespace LedControllerSoftwareMk5
{

    class LedDriver
    {
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        Network *network;
        bool init = false;

    public:
        LedDriver(uint8_t i2cAddress, I2C *i2c, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif