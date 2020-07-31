#ifndef LED_DRIVER_H_INCLUDE
#define LED_DRIVER_H_INCLUDE

#include <Arduino.h>
#include "I2C.h"
#include "PirReader.h"
#include "Network.h"
#include "PCA9685_LED_Reg.h"
#include "Enums.h"
#include "Structs.h"

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
        MotionData motionData;
        LEDStripData curDataStrip1;
        LEDStripData curDataStrip2;
        LEDStripData prevDataStrip1;
        LEDStripData prevDataStrip2;
        bool init = false;
        bool updateStrip1Reg = false;
        bool updateStrip2Reg = false;

    public:

        // ## Functions ## //
    private:
        void HandleLEDStrip(    uint8_t stripID,
                                uint8_t colorFadeSpeed,
                                uint8_t brightnessFadeSpeed,
                                MotionData motionData,
                                LEDStripData curDataStrip,
                                LEDStripData *prevDataStrip);

        void UpdateLEDChannel(  uint8_t i2cAddress,
                                uint8_t REG_ON_L,
                                uint8_t REG_ON_H,
                                uint8_t REG_OFF_L,
                                uint8_t REG_OFF_H,
                                uint16_t phaseShift,
                                uint8_t colorValue, 
                                uint16_t brightnessValue);

        bool FadeToColor(   uint8_t stripID,
                            uint8_t colorFadeSpeed,
                            uint8_t brightnessFadeSpeed,
                            LEDStripData curDataStrip,
                            LEDStripData *prevDataStrip);

        bool FadeToBlack(   uint8_t stripID,
                            uint8_t colorFadeSpeed,
                            uint8_t brightnessFadeSpeed,
                            LEDStripData curDataStrip,
                            LEDStripData *prevDataStrip);

        bool TransitionToNewEffect( uint8_t stripID,
                                    uint8_t colorFadeSpeed,
                                    uint8_t brightnessFadeSpeed,
                                    LEDStripData curDataStrip,
                                    LEDStripData *prevDataStrip);

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