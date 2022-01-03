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
private:
    const String symbol = "=";
    const uint8_t spacerLength = 40; // x * Spaces
    const uint8_t insertLength = 4;  // x * Spaces
public:
    // ================ Methods ================ //
private:
public:
    // ======== Helper Method ======== //
    // == Print
    void TopSpacerPrint();
    void BottomSpacerPrint();
    void InsertPrint();
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
    // == Other
    String BoolToString(bool b);
    String BollToConnectionState(bool b);
    String IntToClientStateInfo(int i);
    String FadeCurveToString(FadeCurve curve);
};
