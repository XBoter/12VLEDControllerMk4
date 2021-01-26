#pragma once

// Includes
#include <Arduino.h>
#include "Network.h"
#include "PirReader.h"
#include "Enums.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class Information : public IBaseClass
{
    // ## Constructor ## //
public:
    Information(Network *network, PirReader *pirReader);

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    PirReader *pirReader;                        // Pointer to used pirReader instance
    PirReader memPirReader = PirReader(0, 0, 0); // Memory version of pirReader instance
    Network *network;                            // Pointer to used network instance
    Network memNetwork = Network();              // Memory version of network instance
    const String symbol = "=";
    const uint8_t spacerLength = 40; // x * Spaces
    const uint8_t insertLength = 4;  // x * Spaces
public:
    // ## Functions ## //
private:
    // Format print functions
    void FormatPrintSingle(String name,
                           String paramterValue);

    void FormatPrintTime(String name,
                         String hour,
                         String minute,
                         String second);

    void FormatPrintLEDStrip(String name,
                             String power,
                             String red,
                             String green,
                             String blue,
                             String colorFadeTime,
                             String colorFadeCurve,
                             String colorBrightness,
                             String colorBrightnessFadeTime,
                             String colorBrightnessFadeCurve,
                             String whiteTemperature,
                             String whiteTemperatureFadeTime,
                             String whiteTemperatureFadeCurve,
                             String whiteBrightness,
                             String whiteBrightnessFadeTime,
                             String whiteBrightnessFadeCurve,
                             String effect);

    void FormatPrintMotion(String name,
                           String motionDetectionEnabled,
                           String timeBasedBrightnessChangeEnabled,
                           String timeout,
                           String red,
                           String green,
                           String blue,
                           String colorBrightness,
                           String whiteTemperature,
                           String whiteBrightness);

    void FormatPrintMotionDetected(String motionDetected,
                                   String sensorTriggered,
                                   String sensor1Triggered,
                                   String sensor2Triggered,
                                   String virtualSensorTriggered);

    // Help functions
    void TopSpacerPrint();
    void BottomSpacerPrint();
    void InsertPrint();

public:
    // Help functions
    String BoolToString(bool b);
    String SingleLEDEffectToString(SingleLEDEffect effect);
    SingleLEDEffect StringToSingleLEDEffect(String effect);
    String MultiLEDEffectToString(MultiLEDEffect effect);
    String FadeCurveToString(FadeCurve curve);
};