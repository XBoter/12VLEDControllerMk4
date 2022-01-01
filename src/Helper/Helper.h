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
    uint8_t convertLEDOutputTypeToUint8(LEDOutputType value);
    LEDOutputType convertUint8ToLEDOutputType(uint8_t value);
    uint8_t convertSingleLEDEffectToUint8(SingleLEDEffect value);
    SingleLEDEffect convertUint8ToSingleLEDEffect(uint8_t value);
    void TopSpacerPrint();
    void BottomSpacerPrint();
    void InsertPrint();
    String BoolToString(bool b);
    String BollToConnectionState(bool b);
    String IntToClientStateInfo(int i);
    String SingleLEDEffectToString(SingleLEDEffect effect);
    SingleLEDEffect StringToSingleLEDEffect(String effect);
    String MultiLEDEffectToString(MultiLEDEffect effect);
    String FadeCurveToString(FadeCurve curve);
};
