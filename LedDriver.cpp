#include "LedDriver.h"

using namespace LedControllerSoftwareMk5;

LedDriver::LedDriver(uint8_t i2cAddress, I2C *i2c, Network *network)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
}

void LedDriver::Init()
{
    if (!init)
    {
        i2c->Init();
        Serial.println("LED Driver initialized");
        init = true;
    }
}

void LedDriver::Run()
{
    if(!init)
    {
        return;
    }

     // Check if WiFi and Mqtt is connected
    if(network->wifiConnected && network->mqttConnected)
    {
        
    }
    else
    {
        
    }
}