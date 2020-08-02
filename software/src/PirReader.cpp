#include "../include/PirReader.h"
#include "../Main.h"


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
bool PirReader::Init()
{
    if(!init)
    {
        // PIR Sensor 1
        pinMode(pinPirSensor1, INPUT);

        // PIR Sensor 2
        pinMode(pinPirSensor2, INPUT);

        Serial.println("PIR Reader initialized");
        init = true;
    }

    return init;
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

    // Check Motion Sensor 1
    if (digitalRead(pinPirSensor1) == HIGH) {
        sensor1Triggered = true;
    } else {
        sensor1Triggered = false;
    }

    // Check Motion Sensor 2
    if (digitalRead(pinPirSensor2) == HIGH) {
        sensor2Triggered = true;
    } else {
        sensor2Triggered = false;
    }

    // Check if motion detected
    if (sensor1Triggered || sensor2Triggered) {
        motionDetected = true;
    }
    if (!sensor1Triggered && !sensor2Triggered) {
        motionDetected = false;
    }

    // Publish motion detected to mqtt
    if(motionDetected != memMotionDetected)
    {
        this->network->MotionDetectedUpdate(motionDetected);
        memMotionDetected = motionDetected;
    }

};
