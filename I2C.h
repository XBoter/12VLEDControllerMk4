#ifndef I2C_H_INCLUDE
#define I2C_H_INCLUDE

#include <Arduino.h>
#include <Wire.h>

namespace LedControllerSoftwareMk5
{

    class I2C
    {
    private:
        bool init = false;

    public:
        I2C();
        void Init();
        uint8_t read8(uint8_t i2cAddress, uint8_t regAddress);
        uint16_t read16(uint8_t i2cAddress, uint8_t regAddress);
        void write8(uint8_t i2cAddres, uint8_t regAddress, uint8_t data);
    };

} // namespace LedControllerSoftwareMk5

#endif