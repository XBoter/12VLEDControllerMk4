#ifndef PIR_READER_H_INCLUDE
#define PIR_READER_H_INCLUDE

#include <Arduino.h>
#include "Network.h"

namespace LedControllerSoftwareMk5
{

    class PirReader
    {
        // ## Data ## //
    private:
        Network *network;
        bool init = false;

    public:
        uint8_t pinPirSensor1 = 0;
        uint8_t pinPirSensor2 = 0;
        bool motionDetected;        // Indicates if motion is detected 
        bool sensor1Triggered;      // Indicates if sensor 1 got triggered
        bool sensor2Triggered;      // Indicates if sensor 2 got triggered
        

        // ## Functions ## //
    private:

    public:
        PirReader(uint8_t pinPirSensor1, uint8_t pinPirSensor2, Network *network);
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif