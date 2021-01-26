#pragma once

// Includes
#include <Arduino.h>
#include "Network.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class PirReader : public IBaseClass
{
    // ## Constructor ## //
public:
    PirReader(uint8_t pinPirSensor1,
              uint8_t pinPirSensor2,
              Network *network);

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    Network *network;
    unsigned long prevMillisMotion = 0;

public:
    uint8_t pinPirSensor1 = 0;
    uint8_t pinPirSensor2 = 0;
    bool motionDetected = false;         // Indicates if motion is detected
    bool sensorTriggered = false;        // Indicates if at least one sensor got triggered
    bool sensor1Triggered = false;       // Indicates if sensor 1 got triggered
    bool sensor2Triggered = false;       // Indicates if sensor 2 got triggered
    bool virtualSensorTriggered = false; // Indicates if the virtual sensor got triggered

    // ## Functions ## //
private:
public:
};