#include "LedDriver.h"

using namespace LedControllerSoftwareMk5;

LedDriver::LedDriver(uint8_t i2cAddress, I2C *i2c, Network *network)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
}


/**
 * Initializes the LedDriver instance
 * @parameter None
 * @return None
 **/
void LedDriver::Init()
{
    if (!init)
    {
        i2c->Init();

        // PWM Prescaler

        uint8_t mode_reg = i2c->read8(i2cAddress, 0x00);
        Serial.print("Mode Reg : ");
        Serial.println(mode_reg, BIN);

        Serial.println("LED Driver initialized");
        init = true;
    }
}


/**
 * Needs to get called every cycle. 
 * Handels the control of the led strips from the PCA9685PW
 * @parameter None
 * @return None
 **/
void LedDriver::Run()
{
    if(!init)
    {
        return;
    }

    HandleLEDStrip1();
    HandleLEDStrip2();

};


/**
 * Handels the control of the 1 LED strip
 * @parameter None
 * @return None
 **/
void LedDriver::HandleLEDStrip1()
{
    // Placeholder
    bool power          = network->parameter_led_strip_1_power;
    uint8_t brightness  = network->parameter_led_strip_1_brightness;
    uint8_t cw          = network->parameter_led_strip_1_cold_white_value;
    uint8_t ww          = network->parameter_led_strip_1_warm_white_value;
    uint8_t red         = network->parameter_led_strip_1_red_value;
    uint8_t green       = network->parameter_led_strip_1_green_value;
    uint8_t blue        = network->parameter_led_strip_1_blue_value;

    // Check for Power
    if(power)
    {
        if(updateStrip1Reg)
        {
            Serial.println("Power On");

            i2c->write8(i2cAddress, LED3_ON_L, 0b11111111);
            i2c->write8(i2cAddress, LED3_ON_H, 0b00001111);

            i2c->write8(i2cAddress, LED3_OFF_L, 0b00000000);
            i2c->write8(i2cAddress, LED3_OFF_H, 0b00000000);

            i2c->write8(i2cAddress, LED4_ON_L, 0b11111111);
            i2c->write8(i2cAddress, LED4_ON_H, 0b00001111);

            i2c->write8(i2cAddress, LED4_OFF_L, 0b00000000);
            i2c->write8(i2cAddress, LED4_OFF_H, 0b00000000);

            updateStrip1Reg = false;
        }
       
    }
    else
    {
        if(!updateStrip1Reg)
        {
            Serial.println("Power Off");

            i2c->write8(i2cAddress, LED3_ON_L, 0b00000000);
            i2c->write8(i2cAddress, LED3_ON_H, 0b00000000);

            i2c->write8(i2cAddress, LED3_OFF_L, 0b00000000);
            i2c->write8(i2cAddress, LED3_OFF_H, 0b00010000);

            i2c->write8(i2cAddress, LED4_ON_L, 0b00000000);
            i2c->write8(i2cAddress, LED4_ON_H, 0b00000000);

            i2c->write8(i2cAddress, LED4_OFF_L, 0b00000000);
            i2c->write8(i2cAddress, LED4_OFF_H, 0b00010000);

            updateStrip1Reg = true;
        }
       
    }
    
};


/**
 * Handels the control of the 2 LED strip
 * @parameter None
 * @return None
 **/
void LedDriver::HandleLEDStrip2()
{

};


void LedDriver::UpdateLEDChannel(uint8_t address, uint8_t value)
{
    // LED_ON_REG 12Bit 0000h - 0FFFh == 0 - 4095
    // LED_OFF_REG 12Bit 0000h - 0FFFh == 0 - 4095
    // With Delay get phase shift

};