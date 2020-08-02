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
        bool memMotionDetected = false;

    public:
        uint8_t pinPirSensor1 = 0;
        uint8_t pinPirSensor2 = 0;
        bool motionDetected;   // Indicates if motion is detected
        bool sensor1Triggered; // Indicates if sensor 1 got triggered
        bool sensor2Triggered; // Indicates if sensor 2 got triggered

    // ## Functions ## //
    private:
    public:

};