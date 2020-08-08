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

        // Master power
        bool master_power = false;

        // LED Strip Refresh rate
        unsigned long previousMillisRefreshRate = 0;
        uint16_t intervalRefreshRate = 0;
        double LED_STRIP_REFRESH_RATE = 90;  // x Times per Second

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

        // LED Strip data
        /*
            Current data of the led strip => Gets displayed
        */
        // Current
        LEDStripData currentLEDStrip1Data; // Gets set to new value
        LEDStripData currentLEDStrip2Data; // Gets set to new value
        // Previous
        LEDStripData prevLEDStrip1Data;
        LEDStripData prevLEDStrip2Data;

        // LED Effect data
        LEDEffectData strip1LEDEffectData;
        LEDEffectData strip2LEDEffectData;

        // Control mode data
        ControlModeData strip1ControlModeData;
        ControlModeData strip2ControlModeData;

        // Default Values
        HighLevelLEDStripData defaultHighLevelLEDStripData;

        // Effect Alarm
    public:

    // ## Functions ## //
    private:
        void HandleControllerMode(uint8_t stripID,
                                  NetworkLEDStripData commandNetworkLEDStripData);

        void HandleLEDEffect(uint8_t stripID,
                             NetworkLEDStripData commandNetworkLEDStripData);

        uint16_t getTimeBasedBrightness();

        // ---- SetColor
        bool SetColor(uint8_t stripID,
                      NetworkLEDStripData commandNetworkLEDStripData);

        bool SetColor(uint8_t stripID,
                      HighLevelLEDStripData commandHighLevelLEDStripData);

        // ---- FadeToColor
        bool FadeToColor(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToColor(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData);

        bool FadeToColor(uint8_t stripID,
                         LowLevelLEDStripData commandLowLevelLEDStripData);

        // ---- FadeToBlack
        bool FadeToBlack(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToBlack(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData);

        bool FadeAllStripsToBlack(uint8_t stripID,
                                  NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeAllStripsToBlack(uint8_t stripID,
                                  HighLevelLEDStripData commandHighLevelLEDStripData);

        // ---- LED Strip
        void UpdateLEDStrip(uint8_t stripID);

        void UpdateLEDChannel(LEDColorReg REG,
                              uint16_t phaseShift,
                              uint8_t colorValue,
                              uint16_t brightnessValue);

        uint16_t getCurveValue(FadeCurve curve,
                               double percent,
                               int start,
                               int end);

        HighLevelLEDStripData getDefaultHigh();

        LowLevelLEDStripData getDefaultLow();

        ControlModeData* getControlModeData(uint8_t stripID);

        void ResetEffectData(uint8_t stripID);

        LEDEffectData* getStripLEDEffectData(uint8_t stripID);

        LEDStripData* getCurrentLEDStripData(uint8_t stripID);

        LEDStripData* getPreviousLEDStripData(uint8_t stripID);

        LEDStripColorReg getColorRegForLEDStrip(uint8_t stripID);

        void PrintAllRegister();

        void PrintByte(byte b);

        uint16_t linear(double percent,
                        int start,
                        int end);

        uint16_t easeInQuart(double percent,
                             int start,
                             int end);

        uint16_t easeOutQuart(double percent,
                              int start,
                              int end);

        uint16_t easeInOutQuart(double percent,
                                int start,
                                int end);

    public:

};
