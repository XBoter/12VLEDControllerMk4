#include "../include/I2C.h"


/**
 * Empty constructor
 */
I2C::I2C()
{

}


/**
 * Does init stuff for the I2C component
 * 
 * @return True if successfull, false if not 
 */
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
 * Runs the I2C component. !! Not used for now !!
 */
void I2C::Run()
{

};


/**
 * Reads 8 bit data from a i2c device 8 bit reg
 * 
 * @parameter i2cAddres     The 8 Bit address of the I2C device
 * @parameter regAddress    The 8 Bit address of the 8 Bit register to read from
 * 
 * @return 8 Bit data from the register if successfull, or 0 if not successfull
 **/
uint8_t I2C::read8(uint8_t i2cAddress, 
                   uint8_t regAddress)
{
    // Send register data request to i2c device
    Wire.beginTransmission(i2cAddress);
    Wire.write(regAddress);
    uint8_t result = Wire.endTransmission();
    bool transmissionGood = checkTransmissionError(result);

    // Check if transmission was good
    if(transmissionGood)
    {
        // Request data from i2c device
        uint8_t result = Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)1);
        int data = Wire.read();
        if(data == -1)
        {
            return 0;
        }
        else
        {
            return (uint8_t)data;
        }
    }
    else
    {
        return 0;
    }

};


/**
 * Reads 16 bit data from a i2c device 16 bit reg
 * 
 * @parameter i2cAddres     The 8 Bit address of the I2C device
 * @parameter regAddress    The 8 Bit address of the 16 Bit register to read from
 * 
 * @return 16 Bit data from the register if successfull, or 0 if not successfull
 **/
uint16_t I2C::read16(uint8_t i2cAddress, 
                     uint8_t regAddress)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(regAddress);
    uint8_t result = Wire.endTransmission();
    bool transmissionGood = checkTransmissionError(result);

    // Check if transmission was good
    if(transmissionGood)
    {
        // Request data from i2c device
        Wire.requestFrom((uint8_t)i2cAddress, (uint8_t)2);
        int MSB = Wire.read();
        int LSB = Wire.read();

        if(MSB == -1 || LSB == -1)
        {
            return 0;
        }
        else
        {
            uint16_t value = (((uint8_t)MSB << 8) | (uint8_t)LSB);
            return value;
        }
    }
    else
    {
        return 0;
    }
};


/**
 * Writes 8 bit of data to a i2c device 8 bit register
 * 
 * @parameter i2cAddres     The 8 Bit address of the I2C device
 * @parameter regAddress    The 8 Bit address of the 8 Bit register to write to
 * @parameter data          The 8 Bit data to write to the register
 * 
 * @return True if successfull, false if not
 **/
bool I2C::write8(uint8_t i2cAddres, 
                 uint8_t regAddress, 
                 uint8_t data)
{
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(data);
    uint8_t result = Wire.endTransmission();
    return checkTransmissionError(result);
};


/**
 * Writes 16 bit of data to a i2c device 16 bit register
 * 
 * @parameter i2cAddres     The 8 Bit address of the I2C device
 * @parameter regAddress    The 8 Bit address of the 16 Bit register to write to
 * @parameter data          The 16 Bit data to write to the register
 * 
 * @return True if successfull, false if not
 **/
bool I2C::write16(uint8_t i2cAddres, 
                  uint8_t regAddress, 
                  uint16_t data)
{
    Wire.beginTransmission(i2cAddres);
    Wire.write(regAddress);
    Wire.write(highByte(data));
    Wire.write(lowByte(data));
    uint8_t result = Wire.endTransmission();
    return checkTransmissionError(result);
};



/**
 * Checks for errors in the result of a Wire.endTransmission()
 * 
 * @parameter result The result of the Wire.endTransmission()
 * 
 * @return True if successfull, or false if not
 */ 
bool I2C::checkTransmissionError(uint8_t result)
{
    bool success = false;
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