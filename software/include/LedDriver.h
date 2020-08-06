#pragma once

// Includes
#include <Arduino.h>
#include "I2C.h"
#include "PirReader.h"
#include "Network.h"
#include "PCA9685_LED_Reg.h"
#include "Enums.h"
#include "Structs.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class LedDriver : public IBaseClass
{
    // ## Constructor ## //
    public:
        LedDriver(uint8_t i2cAddress,
                  I2C *i2c,
                  Network *network,
                  PirReader *pirReader);

    // ## Interface ## //
    private:
    public:
        virtual bool Init();
        virtual void Run();

    // ## Data ## //
    private:
        uint8_t i2cAddress;
        I2C *i2c;
        PirReader *pirReader;
        Network *network;

        // Network Motion data
        NetworkMotionData networkMotionData;
        
        // LED Strip command data
        /*
            New data for the led strip to change to
        */
        NetworkLEDStripData commandNetworkLEDStrip1Data;
        NetworkLEDStripData commandNetworkLEDStrip2Data;

        HighLevelLEDStripData commandHighLevelLEDStrip1Data; 
        HighLevelLEDStripData commandHighLevelLEDStrip2Data;

        LowLevelLEDStripData commandLowLevelLEDStrip1Data; 
        LowLevelLEDStripData commandLowLevelLEDStrip2Data;

        // LED Strip state data
        /*
            Current data of the led strip => Gets displayed
        */
        LowLevelLEDStripData stateLowLevelLEDStrip1Data;
        LowLevelLEDStripData stateLowLevelLEDStrip2Data;
        
        // Default Values
        /*
            Default values for some parameters if not specifed
        */

        // Effect Alarm
    public:

    // ## Functions ## //
    private:
        void HandleLEDStrip(uint8_t stripID,
                            NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToColor(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToColor(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData);

        bool FadeToColor(uint8_t stripID,
                         LowLevelLEDStripData commandLowLevelLEDStripData);

        bool FadeToBlack(uint8_t stripID);

        void UpdateLEDChannel(LEDColorReg REG,
                              uint16_t phaseShift,
                              uint8_t colorValue,
                              uint16_t brightnessValue);

        LowLevelLEDStripData* getLowLevelLEDStripDataOfStrip(uint8_t stripID);
        LEDStripColorReg getColorRegForLEDStrip(uint8_t stripID);
        void PrintAllRegister();
        void PrintByte(byte b);
    public:

};
