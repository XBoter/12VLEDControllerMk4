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

// Blueprint for compiler. Problem => circular dependency
class I2C;
class Network;
class PirReader;

// Classes
class LedDriver : public IBaseClass
{
    // ## Constructor / Important ## //
public:
    LedDriver(uint8_t i2cAddress);
    void setReference(I2C *i2c,
                      Network *network,
                      PirReader *pirReader);
    bool init = false;

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
    double LED_STRIP_REFRESH_RATE = 90; // x Times per Second
    unsigned long refreshRateCounter = 0;

    // ---- Motion Brightness
    TimeBasedMotionBrightness stTimeBasedMotionBrightness = {};

    // ---- Network data
    NetworkMotionData networkMotionData = {};
    NetworkLEDStripData networkLEDStrip1Data = {};
    NetworkLEDStripData networkLEDStrip2Data = {};
    unsigned long prevMillisReconnect = 0;
    unsigned long timeoutReconnect = 4000;

    // ---- LED Effect data
    // -- Multi
    MultiLEDStripEffectData multiLEDStripEffectData = {};

    // -- Single
    SingleLEDStripEffectData emptySingleLEDStripEffectData = {};
    SingleLEDStripEffectData singleLEDStrip1EffectData = {};
    SingleLEDStripEffectData singleLEDStrip2EffectData = {};

    // ---- LED Strip command data
    // -- High Level
    HighLevelLEDStripData highLevelLEDStrip1Data = {};
    HighLevelLEDStripData highLevelLEDStrip2Data = {};
    // -- Low Level
    LowLevelLEDStripData lowLevelLEDStrip1Data = {};
    LowLevelLEDStripData lowLevelLEDStrip2Data = {};

    // ---- LED Strip data
    // -- Current
    LEDStripData emptyCurrentLEDStripData = {};
    LEDStripData currentLEDStrip1Data = {};
    LEDStripData currentLEDStrip2Data = {};
    // -- Previous
    LEDStripData emptyPrevLEDStripData = {};
    LEDStripData prevLEDStrip1Data = {};
    LEDStripData prevLEDStrip2Data = {};

public:
    // ## Functions ## //
    bool ConfigureMode();

private:
    // ---- Logic
    void HandleMultiLEDStripControlLogic();

    // ---- Effects
    void HandleMultiLEDStripEffects();

    void HandleSingleLEDStripEffects(uint8_t stripID,
                                     NetworkLEDStripData commandNetworkLEDStripData);

    uint8_t getMotionBrightnessPercent();

    MultiLEDStripEffectData *getMultiLEDStripEffectData();
    MultiLEDEffect prevMultiLEDEffect = MultiLEDEffect::Idle;

    SingleLEDStripEffectData *getSingleLEDStripEffectData(uint8_t stripID);

    // ---- Data Types and defaults
    // -- Default types for FadeTimes and FadeCurves
    HighLevelLEDStripData defaultHighLevelFadeTimesAndFadeCurves = {};
    LowLevelLEDStripData defaultLowLevelFadeTimesAndFadeCurves = {};
    void createInitalTypes(); // Gets called on init
    // -- FadeTimes and FadeCurves
    // Currents
    HighLevelLEDStripData currentHighLevelFadeTimesAndFadeCurves = {};
    LowLevelLEDStripData currentLowLevelFadeTimesAndFadeCurves = {};
    HighLevelLEDStripData getHighLevelFadeTimesAndCurves();
    LowLevelLEDStripData getLowLevelFadeTimesAndCurves();
    // Instant
    HighLevelLEDStripData instantHighLevelFadeTimesAndCurves = {};
    LowLevelLEDStripData instantLowLevelFadeTimesAndCurves = {};
    HighLevelLEDStripData getInstantHighLevelFadeTimesAndCurves();
    LowLevelLEDStripData getInstantLowLevelFadeTimesAndCurves();
    // -- Conversions and combine
    /*
        Data Level Hierarchy

        Top     NetworkLEDStripData
        Middle  HighLevelLEDStripData
        Low     LowLevelLEDStripData
    */
    HighLevelLEDStripData convertNetworkDataToHighLevelData(NetworkLEDStripData networkData,
                                                            HighLevelLEDStripData highLevelFadeTimesAndCurves);
    LowLevelLEDStripData convertHighLevelDataToLowLevelData(HighLevelLEDStripData highLevelLEDStripData,
                                                            LowLevelLEDStripData lowLevelFadeTimesAndCurves);
    LowLevelLEDStripData combineLowLevelDataToLowLevelData(LowLevelLEDStripData lowLevelLEDStripData,
                                                           LowLevelLEDStripData lowLevelFadeTimesAndCurves);

    // ---- SetColor
    // -- Single Strip
    bool SetColor(uint8_t stripID,
                  NetworkLEDStripData commandNetworkLEDStripData);

    bool SetColor(uint8_t stripID,
                  HighLevelLEDStripData commandHighLevelLEDStripData);

    bool SetColor(uint8_t stripID,
                  LowLevelLEDStripData commandHighLevelLEDStripData);

    // -- Multi Strip
    bool SetColor(NetworkLEDStripData commandNetworkLEDStripData);

    bool SetColor(HighLevelLEDStripData commandHighLevelLEDStripData);

    bool SetColor(LowLevelLEDStripData commandLowLevelLEDStripData);

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
    bool FadeToBlack(uint8_t stripID);

    // -- Multi Strip
    bool FadeToBlack();

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

    LEDStripData *getCurrentLEDStripData(uint8_t stripID);

    LEDStripData *getPreviousLEDStripData(uint8_t stripID);

    LEDStripColorReg getColorRegForLEDStrip(uint8_t stripID);

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
