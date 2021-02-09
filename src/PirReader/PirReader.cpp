#include "PirReader.h"

/**
 * Constructor for the PirReader class
 * 
 * @parameter pinPirSensor1     The digital pin of the 1 PIR sensor
 * @parameter pinPirSensor2     The digital pin of the 2 PIR sensor
 * @parameter *network          Pointer to the used instace of the Network class
 */
PirReader::PirReader(uint8_t pinPirSensor1,
                     uint8_t pinPirSensor2)
{
    this->pinPirSensor1 = pinPirSensor1;
    this->pinPirSensor2 = pinPirSensor2;
};

/**
 * Sets reference to external components
 */
void PirReader::setReference(Network *network)
{
    this->network = network;
};

/**
 * Does init stuff for the PirReader component
 * 
 * @return True if successfull, false if not 
 */
bool PirReader::Init()
{
    if (!init)
    {
        // PIR Sensor 1
        pinMode(pinPirSensor1, INPUT);

        // PIR Sensor 2
        pinMode(pinPirSensor2, INPUT);

        Serial.println(F("PIR Reader initialized"));
        init = true;
    }

    return init;
};

/**
 * Runs the PirReader component. 
 */
void PirReader::Run()
{
    if (!init)
    {
        return;
    }

    // Check Physical Motion Sensor 1
    if (digitalRead(pinPirSensor1) == HIGH)
    {
        sensor1Triggered = true;
    }
    else
    {
        sensor1Triggered = false;
    }

    // Check Physical Motion Sensor 2
    if (digitalRead(pinPirSensor2) == HIGH)
    {
        sensor2Triggered = true;
    }
    else
    {
        sensor2Triggered = false;
    }

    // Check Virtual Motion Sensor
    if (network->virtualPIRSensorTriggered)
    {
        virtualSensorTriggered = true;
    }
    else
    {
        virtualSensorTriggered = false;
    }

    // Check if motion detected by physical or virtual sensor
    if (sensor1Triggered || sensor2Triggered || virtualSensorTriggered)
    {
        sensorTriggered = true;
        motionDetected = true;
        prevMillisMotion = millis();
    }
    if (!sensor1Triggered && !sensor2Triggered && !virtualSensorTriggered)
    {
        sensorTriggered = false;
    }

    // Update motionDetected based on timeout
    if (millis() - prevMillisMotion >= (network->stNetworkMotionData.timeout * 1000))
    {
        motionDetected = false;
    }
}
