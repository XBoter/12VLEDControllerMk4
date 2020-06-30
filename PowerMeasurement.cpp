#include "PowerMeasurement.h"

using namespace LedControllerSoftwareMk5;

PowerMeasurement::PowerMeasurement(uint8_t i2cAddress, I2C *i2c, Network *network)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
}

void PowerMeasurement::Init()
{
    if (!init)
    {
        i2c->Init();

        // Calibrate INA219

        Serial.println("Power Measurement Unit initialized");
        init = true;
    }
}

void PowerMeasurement::Run()
{
    // Check if WiFi or Mqtt is connected
    if (network->wifiConnected || network->mqttConnected)
    {
        unsigned long CurMillis_PowerMessurmentUpdateRate = millis();
        if (CurMillis_PowerMessurmentUpdateRate - PrevMillis_PowerMessurmentUpdateRate >= TimeOut_PowerMessurmentUpdateRate)
        {
            PrevMillis_PowerMessurmentUpdateRate = CurMillis_PowerMessurmentUpdateRate;


            
        }
    }
    else
    {
    }
}