#include "I2C.h"

using namespace LedControllerSoftwareMk5;

I2C::I2C()
{
}

void I2C::Init()
{
    if (!init)
    {
        Wire.begin();
        Serial.println("I2C initialized");
        init = true;
    }
}

void I2C::Run()
{

}

uint8_t I2C::read8(uint8_t i2cAddress, uint8_t regAddress)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(regAddress);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
    return Wire.read();
}

uint16_t I2C::read16(uint8_t i2cAddress, uint8_t regAddress)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(regAddress);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)2);
    uint8_t MSB = Wire.read();
    uint8_t LSB = Wire.read();

    uint16_t value = ((MSB << 8) | LSB);
    return value;
}

void I2C::write8(uint8_t i2cAddres, uint8_t regAddress, uint8_t data)
{
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void I2C::write16(uint8_t i2cAddres, uint8_t regAddress, uint16_t data)
{
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(highByte(data));
    Wire.write(lowByte(data));
    Wire.endTransmission();
}