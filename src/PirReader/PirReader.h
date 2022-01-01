#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include "../Network/Network.h"
#include "../Information/Information.h"
#include "../Helper/Helper.h"

#include "../Structs/Structs.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Network;
class Information;
class Helper;

// ================================ CLASS ================================ //
/**
 * @brief The PirReader Class handles evaluation of the PIR sensor's
 * 
 */
class PirReader : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    PirReader(uint8_t pinPirSensor1,
              uint8_t pinPirSensor2);
    void setReference(Network *network,
                      Information *information,
                      Helper *helper);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    Network *network;
    Information *information;
    Helper *helper;
    unsigned long prevMillisMotion = 0;
    uint8_t pinPirSensor1 = 0;
    uint8_t pinPirSensor2 = 0;
    bool motionDetected = false; // Indicates if motion is detected
    bool memMotionDetected = false;
    bool sensorTriggered = false; // Indicates if at least one sensor got triggered
    bool memSensorTriggered = false;
    bool sensor1Triggered = false;       // Indicates if sensor 1 got triggered
    bool sensor2Triggered = false;       // Indicates if sensor 2 got triggered
    bool virtualSensorTriggered = false; // Indicates if the virtual sensor got triggered

public:
    // ================ Methods ================ //
private:
public:
    bool MotionDetected();
    PIRReaderData getPIRReaderData();
};