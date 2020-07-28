#include "PirReader.h"
#include "Network.h"
#include "Main.h"

using namespace LedControllerSoftwareMk5;

// For compiler
ICACHE_RAM_ATTR void StateMotionDetectedSensor1();
ICACHE_RAM_ATTR void StateMotionDetectedSensor2();


PirReader::PirReader(uint8_t pinPirSensor1, uint8_t pinPirSensor2, Network *network)
{
    this->pinPirSensor1 = pinPirSensor1;
    this->pinPirSensor2 = pinPirSensor2;
    this->network = network;
};


/**
 * Initializes the PirReader instance
 * @parameter None
 * @return None
 **/
void PirReader::Init()
{
    if(!init)
    {
        // PIR Sensor 1
        pinMode(pinPirSensor1, INPUT);
        attachInterrupt(digitalPinToInterrupt(pinPirSensor1), StateMotionDetectedSensor1, CHANGE);

        // PIR Sensor 2
        pinMode(pinPirSensor2, INPUT);
        attachInterrupt(digitalPinToInterrupt(pinPirSensor2), StateMotionDetectedSensor1, CHANGE);

        init = true;
    }
};


/**
 * Needs to get called every cycle. 
 * Handels the read of the pir sensor and motion detection
 * @parameter None
 * @return None
 **/
void PirReader::Run()
{
    if(!init)
    {
        return;
    }

    if (sensor1Triggered || sensor2Triggered) {
        motionDetected = true;
    }
    if (!sensor1Triggered && !sensor2Triggered) {
        motionDetected = false;
    }

};


ICACHE_RAM_ATTR void StateMotionDetectedSensor1()
{
    if (digitalRead(mainController.pirReader.pinPirSensor1) == HIGH) {
        mainController.pirReader.sensor1Triggered = true;
    } else {
        mainController.pirReader.sensor1Triggered = false;
    }
};


ICACHE_RAM_ATTR void StateMotionDetectedSensor2()
{
    if (digitalRead(mainController.pirReader.pinPirSensor2) == HIGH) {
        mainController.pirReader.sensor2Triggered = true;
    } else {
        mainController.pirReader.sensor2Triggered = false;
    }
};