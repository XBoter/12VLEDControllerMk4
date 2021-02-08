#pragma once

// Includes
#include <Arduino.h>
#include <Wire.h>

// Interface
#include "../interface/IBaseClass.h"

// Classes
class I2C : public IBaseClass
{
    // ## Constructor / Important ## //
public:
    I2C();
    void setReference();
    bool init = false;

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
public:
    // ## Functions ## //
private:
    bool checkTransmissionError(uint8_t result);

public:
    // I2C read functions
    uint8_t read8(uint8_t i2cAddress,
                  uint8_t regAddress);

    uint16_t read16(uint8_t i2cAddress,
                    uint8_t regAddress);

    // I2C write functions
    bool write8(uint8_t i2cAddres,
                uint8_t regAddress,
                uint8_t data);

    bool write16(uint8_t i2cAddres,
                 uint8_t regAddress,
                 uint16_t data);
};
