#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include "../Network/Network.h"
#include "../PirReader/PirReader.h"
#include "../Enums/Enums.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Network;
class PirReader;

// ================================ CLASS ================================ //
/**
 * @brief The Information Class prints diffrent information messages to the serial terminal
 * 
 */
class Information : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Information();
    void setReference(Helper *helper);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    Helper *helper;

public:
    // ================ Methods ================ //
private:
public:
    void FormatPrintSingle(String name,
                           String paramterValue);

    void FormatPrintTime(String name,
                         String hour,
                         String minute,
                         String second,
                         String unix);

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

    void FormatPrintNetworkWiFi(String connectionState,
                                String hostname,
                                String ipAddress,
                                String subnetmask,
                                String macAddress,
                                String gateway);

    void FormatPrintNetworkMQTT(String connectionState,
                                String clientStateInfo,
                                String brokerIpAddress,
                                int brokerPort,
                                String clientName);
};
