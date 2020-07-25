#ifndef LED_DRIVER_H_INCLUDE
#define LED_DRIVER_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "Network.h"
#include "PCA9685_LED_Reg.h"

namespace LedControllerSoftwareMk5
{

    class LedDriver
    {
        // ## Data ## //
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        Network *network;
        bool init = false;
        bool updateStrip1Reg = false;
        bool updateStrip2Reg = false;

    public:

        // ## Functions ## //
    private:
        void HandleLEDStrip1();
        void HandleLEDStrip2();
        void UpdateLEDChannel(uint8_t address, uint8_t value);

    public:
        LedDriver(uint8_t i2cAddress, I2C *i2c, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif