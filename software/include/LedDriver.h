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

        // ---- LED Strip Refresh Rate
        unsigned long previousMillisRefreshRate = 0;
        uint16_t intervalRefreshRate = 0;
        double LED_STRIP_REFRESH_RATE = 90;  // x Times per Second
        unsigned long refreshRateCounter = 0;

        // ---- Network data
        NetworkLEDStripData networkLEDStrip1Data;
        NetworkLEDStripData networkLEDStrip2Data;

        // ---- LED Effect data
        // -- Multi
        MultiLEDStripEffectData curMultiLEDStripEffectData;
        MultiLEDStripEffectData prevMultiLEDStripEffectData;
        bool multiLEDStripPower = false; // Indicates if at least one LED strip is powered on 
        // -- Single
        SingleLEDStripEffectData singleLEDStrip1EffectData;
        SingleLEDStripEffectData singleLEDStrip2EffectData;

        // ---- LED Strip command data
        // -- High Level
        HighLevelLEDStripData highLevelLEDStrip1Data; 
        HighLevelLEDStripData highLevelLEDStrip2Data;
        // -- Low Level
        LowLevelLEDStripData lowLevelLEDStrip1Data; 
        LowLevelLEDStripData lowLevelLEDStrip2Data;

        // ---- LED Strip data
        // -- Current
        LEDStripData currentLEDStrip1Data; // Gets set to new value
        LEDStripData currentLEDStrip2Data; // Gets set to new value
        // -- Previous
        LEDStripData prevLEDStrip1Data;
        LEDStripData prevLEDStrip2Data;

        // ---- Default Data
        HighLevelLEDStripData defaultHighLevelLEDStripData;

    public:

    // ## Functions ## //
    private:
        // ---- Effects
        void HandleMultiLEDStripEffects();

        void HandleSingleLEDStripEffects(uint8_t stripID,
                                         NetworkLEDStripData commandNetworkLEDStripData);

        uint16_t getTimeBasedBrightness();

        void ResetSingleEffectData();

        SingleLEDStripEffectData* getSingleLEDStripEffectData(uint8_t stripID);

        // ---- SetColor
        // -- Single Strip
        bool SetColor(uint8_t stripID,
                      NetworkLEDStripData commandNetworkLEDStripData);

        bool SetColor(uint8_t stripID,
                      HighLevelLEDStripData commandHighLevelLEDStripData);
        // -- Multi Strip
        bool SetColor(NetworkLEDStripData commandNetworkLEDStripData);

        bool SetColor(HighLevelLEDStripData commandHighLevelLEDStripData);

        // ---- FadeToColor
        // -- Single Strip
        bool FadeToColor(uint8_t stripID,
                        NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToColor(uint8_t stripID,
                        HighLevelLEDStripData commandHighLevelLEDStripData);

        bool FadeToColor(uint8_t stripID,
                        LowLevelLEDStripData commandLowLevelLEDStripData);
        // -- Multi Strip
        bool FadeToColor(NetworkLEDStripData commandHighLevelLEDStripData);

        bool FadeToColor(HighLevelLEDStripData commandHighLevelLEDStripData);

        bool FadeToColor(LowLevelLEDStripData commandLowLevelLEDStripData);

        // ---- FadeToBlack
        // -- Single Strip
        bool FadeToBlack(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToBlack(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData);

        // -- Multi Strip
        bool FadeToBlack(NetworkLEDStripData commandNetworkLEDStripData);

        bool FadeToBlack(HighLevelLEDStripData commandHighLevelLEDStripData);

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
