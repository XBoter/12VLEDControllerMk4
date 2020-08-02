#include "../include/I2C.h"


I2C::I2C()
{

};


/**
 * Initializes the i2c instance
 * @parameter None
 * @return None
 **/
bool I2C::Init()
{
    if (!init)
    {
        Wire.begin();
        Serial.println("I2C initialized");
        init = true;
    }
    
    return init;
};


/**
 * Needs to get called every cycle.
 * @parameter None
 * @return None
 **/
void I2C::Run()
{

};


/**
 * Reads 8 bit data from a i2c device 8 bit reg
 * @parameter I2C-Address of the device, Register Address to read from
 * @return 1 byte data from the reg
 **/
uint8_t I2C::read8(uint8_t i2cAddress, uint8_t regAddress)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(regAddress);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
    return Wire.read();
};


/**
 * Reads 16 bit data from a i2c device 16 bit reg
 * @parameter I2C-Address of the device, Register Address to read from
 * @return 2 byte data from the reg
 **/
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
};


/**
 * Writes 8 bit of data to a i2c device 8 bit register
 * @parameter I2C-Address of the device, Register Address to be written to, 1 byte of data
 * @return Successfull write or not
 **/
bool I2C::write8(uint8_t i2cAddres, uint8_t regAddress, uint8_t data)
{
    /*
    Serial.print("I2C  : ");
    Serial.println(i2cAddres, HEX);
    Serial.print("REG  : ");
    Serial.println(regAddress, HEX);
    Serial.print("DATA : ");
    Serial.println(data, HEX);
    */
    bool success = false;
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(data);
    uint8_t result = Wire.endTransmission();
    switch (result)
    {
        case 0:
            success = true;
            break;
        case 1:
            Serial.println("I2C Write ERROR! => data too long to fit in transmit buffer");
            success = false;
            break;
        case 2:
            Serial.println("I2C Write ERROR! => received NACK on transmit of address");
            success = false;
            break;
        case 3:
            Serial.println("I2C Write ERROR! => received NACK on transmit of data");
            success = false;
            break;
        case 4:
            Serial.println("I2C Write ERROR! => other error");
            success = false;
            break;
    }
    return success;
};


/**
 * Writes 16 bit of data to a i2c device 16 bit register
 * @parameter I2C-Address of the device, Register Address to be written to, 2 byte of data
 * @return Successfull write or not
 **/
bool I2C::write16(uint8_t i2cAddres, uint8_t regAddress, uint16_t data)
{
    bool success = false;
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(highByte(data));
    Wire.write(lowByte(data));
    uint8_t result = Wire.endTransmission();
    switch (result)
    {
        case 0:
            success = true;
            break;
        case 1:
            Serial.println("I2C Write ERROR! => data too long to fit in transmit buffer");
            success = false;
            break;
        case 2:
            Serial.println("I2C Write ERROR! => received NACK on transmit of address");
            success = false;
            break;
        case 3:
            Serial.println("I2C Write ERROR! => received NACK on transmit of data");
            success = false;
            break;
        case 4:
            Serial.println("I2C Write ERROR! => other error");
            success = false;
            break;
    }
    return success;
};