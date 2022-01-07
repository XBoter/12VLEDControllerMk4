#include "PirReader.h"

/**
 * @brief Constructor for the PirReader class
 * 
 * @param pinPirSensor1 The digital pin of the 1 PIR sensor
 * @param pinPirSensor2 The digital pin of the 2 PIR sensor
 */
PirReader::PirReader(uint8_t pinPirSensor1,
                     uint8_t pinPirSensor2)
{
    this->pinPirSensor1 = pinPirSensor1;
    this->pinPirSensor2 = pinPirSensor2;
};

/**
 * @brief Sets the needed refernce for the pir reader class
 */
void PirReader::setReference(Network *network,
                             Information *information,
                             Helper *helper)
{
    this->network = network;
    this->information = information;
    this->helper = helper;
};

/**
 * @brief Initializes the pir reader component
 * 
 * @return True if the initialization was successful
 */
bool PirReader::Init()
{
    if (!init)
    {
        // PIR Sensor 1
        pinMode(this->pinPirSensor1, INPUT);

        // PIR Sensor 2
        pinMode(this->pinPirSensor2, INPUT);

        Serial.println(F("PIR Reader initialized"));
        init = true;
    }

    return init;
};

/**
 * @brief Runs the pir reader component
 * 
 */
void PirReader::Run()
{
    if (!init)
    {
        return;
    }

    // Check Physical Motion Sensor 1
    if (digitalRead(this->pinPirSensor1) == HIGH)
    {
        this->sensor1Triggered = true;
    }
    else
    {
        this->sensor1Triggered = false;
    }

    // Check Physical Motion Sensor 2
    if (digitalRead(this->pinPirSensor2) == HIGH)
    {
        this->sensor2Triggered = true;
    }
    else
    {
        this->sensor2Triggered = false;
    }

    // Check Virtual Motion Sensor
    if (this->network->isVirtualPIRSensorTriggered())
    {
        this->virtualSensorTriggered = true;
    }
    else
    {
        this->virtualSensorTriggered = false;
    }

    // Check if motion detected by physical or virtual sensor
    if (this->sensor1Triggered || this->sensor2Triggered || this->virtualSensorTriggered)
    {
        this->sensorTriggered = true;
        this->motionDetected = true;
        prevMillisMotion = millis();
    }
    if (!this->sensor1Triggered && !this->sensor2Triggered && !this->virtualSensorTriggered)
    {
        this->sensorTriggered = false;
    }

    // Update motionDetected based on timeout
    if (millis() - prevMillisMotion >= (network->getNetworkMotionData().MotionDetectionTimeout * 1000))
    {
        motionDetected = false;
    }

    if (this->sensorTriggered != this->memSensorTriggered || this->motionDetected != this->memMotionDetected)
    {
        this->information->FormatPrintMotionDetected(this->helper->BoolToString(this->motionDetected),
                                                     this->helper->BoolToString(this->sensorTriggered),
                                                     this->helper->BoolToString(this->sensor1Triggered),
                                                     this->helper->BoolToString(this->sensor2Triggered),
                                                     this->helper->BoolToString(this->virtualSensorTriggered));
        this->memSensorTriggered = this->sensorTriggered;
        this->memMotionDetected = this->motionDetected;
    }
}

/**
 * @brief Indicates if motion is detected
 * 
 * @return True motion was detected via the physical or virtual sensor
 */
bool PirReader::MotionDetected()
{
    return this->motionDetected;
}

/**
 * @brief Retruns the current state of the PIR Reader data
 * 
 * @return PIRReaderData 
 */
PIRReaderData PirReader::getPIRReaderData()
{
    PIRReaderData data = {};

    data.motionDetected = this->motionDetected;
    data.sensorTriggered = this->sensorTriggered;
    data.sensor1Triggered = this->sensor1Triggered;
    data.sensor2Triggered = this->sensor2Triggered;
    data.virtualSensorTriggered = this->virtualSensorTriggered;

    return data;
}