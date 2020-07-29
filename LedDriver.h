#ifndef LED_DRIVER_H_INCLUDE
#define LED_DRIVER_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "PirReader.h"
#include "Network.h"
#include "PCA9685_LED_Reg.h"
#include "Enums.h"

namespace LedControllerSoftwareMk5
{

    class LedDriver
    {
        // ## Data ## //
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        PirReader *pirReader;
        Network *network;
        Network memNetwork;
        bool init = false;
        bool updateStrip1Reg = false;
        bool updateStrip2Reg = false;

    public:

        // ## Functions ## //
    private:
        void HandleLEDStrip(    uint8_t stripID,
                                bool power,
                                uint8_t brightness,
                                uint8_t cw,
                                uint8_t ww,
                                uint8_t red,
                                uint8_t green,
                                uint8_t blue,
                                LEDEffect effect);

        void UpdateLEDChannel(  uint8_t i2cAddress, 
                                uint8_t regAddress,
                                uint8_t colorValue,
                                uint8_t brightnessValue);
        void PrintAllRegister();
        void PrintByte(byte b);

    public:
        LedDriver(  uint8_t i2cAddress, 
                    I2C *i2c, 
                    Network *network, 
                    PirReader *pirReader);
        void Init();
        void Run();
    };

};

#endif