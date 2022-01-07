#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include "../Structs/Structs.h"
#include "../Enums/Enums.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// ================================ CLASS ================================ //
/**
 * @brief The Helper Class holds usefull functions that are used by all other classes
 */
class Helper : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Helper();
    void setReference();
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();
    // ================ Data ================ //
    // ======== OnBoard LED ======== //
    bool onboardLEDState = false;
    unsigned long prevMillisBlinkOnBoardLED = 0;

private:
    const String symbol = "=";
    const uint8_t spacerLength = 45; // x * Spaces
    const uint8_t insertLength = 3;  // x * Spaces
public:
    // ================ Methods ================ //
private:
public:
    // ======== Helper Method ======== //
    // == OnBoard LED
    void blinkOnBoardLED(uint16_t interval);
    void turnOffOnBoardLED();
    void turnOnOnBoardLED();
    // == Print
    void TopSpacerPrint();
    void BottomSpacerPrint();
    void InsertPrint();
    void SimplePrint(String msg);
    // == MultiLEDEffect
    String MultiLEDEffectToString(MultiLEDEffect effect);
    SingleLEDEffect Uint8ToSingleLEDEffect(uint8_t value);
    uint8_t MultiLEDEffectToUint8(MultiLEDEffect effect);
    // == SingleLEDEffect
    String SingleLEDEffectToString(SingleLEDEffect effect);
    SingleLEDEffect StringToSingleLEDEffect(String effect);
    uint8_t SingleLEDEffectToUint8(SingleLEDEffect value);
    // == LEDOutputType
    String LEDOutputTypeToString(LEDOutputType type);
    LEDOutputType Uint8ToLEDOutputType(uint8_t value);
    uint8_t LEDOutputTypeToUint8(LEDOutputType type);
    // == FadeCurve
    String FadeCurveToString(FadeCurve curve);
    FadeCurve Uint8ToFadeCurve(uint8_t value);
    uint8_t FadeCurveToUint8(FadeCurve type);
    // == Other
    String BoolToString(bool b);
    String BollToConnectionState(bool b);
    String IntToClientStateInfo(int i);
};
