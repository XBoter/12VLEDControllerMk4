#include "LedDriver.h"

using namespace LedControllerSoftwareMk5;

LedDriver::LedDriver(uint8_t i2cAddress, I2C *i2c, Network *network, PirReader *pirReader)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
    this->pirReader = pirReader;
};


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

        // Reset ic
        i2c->write8(i2cAddress, MODE1, 0b10000000);
        i2c->write8(i2cAddress, MODE1, 0b10001000);
        delay(1000); // min 500 qs delay
        i2c->write8(i2cAddress, MODE1, 0b10000000);
        delay(200);

        // Turn all LEDs of 
        i2c->write8(i2cAddress, ALL_LED_ON_H, 0b00000000);
        i2c->write8(i2cAddress, ALL_LED_OFF_H, 0b00000000);
        AllBlack();

        // Clear Mode 1 register
        i2c->write8(i2cAddress, MODE1, 0b00000000);

        // Set sleep bit to set prescaler
        i2c->write8(i2cAddress, MODE1, 0b00010000);

        // Set prescaler to 1526 hz
        i2c->write8(i2cAddress, PRE_SCALE, 0b00000011);

        // Reset sleep bit after prescaler set
        i2c->write8(i2cAddress, MODE1, 0b00000000);

        // Set Mode 2 register
        i2c->write8(i2cAddress, MODE2, 0b00010100);
        /*
            INVRT = 1
            OUTDRV = 1
        */

        PrintAllRegister();

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

    // Handle led strip 1
    // Check for change in parameter
    if( network->parameter_led_strip_1_power                != memNetwork.parameter_led_strip_1_power
        || network->parameter_led_strip_1_brightness        != memNetwork.parameter_led_strip_1_brightness
        || network->parameter_led_strip_1_cold_white_value  != memNetwork.parameter_led_strip_1_cold_white_value
        || network->parameter_led_strip_1_warm_white_value  != memNetwork.parameter_led_strip_1_warm_white_value
        || network->parameter_led_strip_1_red_value         != memNetwork.parameter_led_strip_1_red_value
        || network->parameter_led_strip_1_green_value       != memNetwork.parameter_led_strip_1_green_value
        || network->parameter_led_strip_1_blue_value        != memNetwork.parameter_led_strip_1_blue_value
        || network->parameter_led_strip_1_effect            != memNetwork.parameter_led_strip_1_effect)
    {
        // Call handle
        HandleLEDStrip( 1,
                        network->parameter_led_strip_1_power,
                        network->parameter_led_strip_1_brightness,
                        network->parameter_led_strip_1_cold_white_value,
                        network->parameter_led_strip_1_warm_white_value,
                        network->parameter_led_strip_1_red_value,
                        network->parameter_led_strip_1_green_value,
                        network->parameter_led_strip_1_blue_value,
                        network->parameter_led_strip_1_effect);
        // Update memory
        memNetwork.parameter_led_strip_1_power              = network->parameter_led_strip_1_power;
        memNetwork.parameter_led_strip_1_brightness         = network->parameter_led_strip_1_brightness ;
        memNetwork.parameter_led_strip_1_cold_white_value   = network->parameter_led_strip_1_cold_white_value ;
        memNetwork.parameter_led_strip_1_warm_white_value   = network->parameter_led_strip_1_warm_white_value ;
        memNetwork.parameter_led_strip_1_red_value          = network->parameter_led_strip_1_red_value;
        memNetwork.parameter_led_strip_1_green_value        = network->parameter_led_strip_1_green_value;
        memNetwork.parameter_led_strip_1_blue_value         = network->parameter_led_strip_1_blue_value;
        memNetwork.parameter_led_strip_1_effect             = network->parameter_led_strip_1_effect;
    }


    // Handle led strip 2
    // Check for change in parameter
    if( network->parameter_led_strip_2_power                != memNetwork.parameter_led_strip_2_power
        || network->parameter_led_strip_2_brightness        != memNetwork.parameter_led_strip_2_brightness
        || network->parameter_led_strip_2_cold_white_value  != memNetwork.parameter_led_strip_2_cold_white_value
        || network->parameter_led_strip_2_warm_white_value  != memNetwork.parameter_led_strip_2_warm_white_value
        || network->parameter_led_strip_2_red_value         != memNetwork.parameter_led_strip_2_red_value
        || network->parameter_led_strip_2_green_value       != memNetwork.parameter_led_strip_2_green_value
        || network->parameter_led_strip_2_blue_value        != memNetwork.parameter_led_strip_2_blue_value
        || network->parameter_led_strip_2_effect            != memNetwork.parameter_led_strip_2_effect)
    {
        // Call handle
        HandleLEDStrip( 2,
                        network->parameter_led_strip_2_power,
                        network->parameter_led_strip_2_brightness,
                        network->parameter_led_strip_2_cold_white_value,
                        network->parameter_led_strip_2_warm_white_value,
                        network->parameter_led_strip_2_red_value,
                        network->parameter_led_strip_2_green_value,
                        network->parameter_led_strip_2_blue_value,
                        network->parameter_led_strip_2_effect);
        // Update memory
        memNetwork.parameter_led_strip_2_power              = network->parameter_led_strip_2_power;
        memNetwork.parameter_led_strip_2_brightness         = network->parameter_led_strip_2_brightness ;
        memNetwork.parameter_led_strip_2_cold_white_value   = network->parameter_led_strip_2_cold_white_value ;
        memNetwork.parameter_led_strip_2_warm_white_value   = network->parameter_led_strip_2_warm_white_value ;
        memNetwork.parameter_led_strip_2_red_value          = network->parameter_led_strip_2_red_value;
        memNetwork.parameter_led_strip_2_green_value        = network->parameter_led_strip_2_green_value;
        memNetwork.parameter_led_strip_2_blue_value         = network->parameter_led_strip_2_blue_value;
        memNetwork.parameter_led_strip_2_effect             = network->parameter_led_strip_2_effect;
    }


};


/**
 * Handels the control of a LED strip
 * @parameter None
 * @return None
 **/
void LedDriver::HandleLEDStrip( uint8_t stripID,
                                bool power,
                                uint8_t brightness,
                                uint8_t cw,
                                uint8_t ww,
                                uint8_t red,
                                uint8_t green,
                                uint8_t blue,
                                LEDEffect effect)
{
    // StripID needs to be given else return
    if(stripID != 1 && stripID != 2)
    {
        return;
    }
    // ---- Register
    // -- CW
    uint8_t CW_REG_ON_L = 0x00;
    uint8_t CW_REG_ON_H = 0x00;
    uint8_t CW_REG_OFF_L = 0x00;
    uint8_t CW_REG_OFF_H = 0x00;

    // -- ww
    uint8_t WW_REG_ON_L = 0x00;
    uint8_t WW_REG_ON_H = 0x00;
    uint8_t WW_REG_OFF_L = 0x00;
    uint8_t WW_REG_OFF_H = 0x00;

    // -- RED
    uint8_t RED_REG_ON_L = 0x00;
    uint8_t RED_REG_ON_H = 0x00;
    uint8_t RED_REG_OFF_L = 0x00;
    uint8_t RED_REG_OFF_H = 0x00;

    // -- GREEN
    uint8_t GREEN_REG_ON_L = 0x00;
    uint8_t GREEN_REG_ON_H = 0x00;
    uint8_t GREEN_REG_OFF_L = 0x00;
    uint8_t GREEN_REG_OFF_H = 0x00;

    // -- BLUE
    uint8_t BLUE_REG_ON_L = 0x00;
    uint8_t BLUE_REG_ON_H = 0x00;
    uint8_t BLUE_REG_OFF_L = 0x00;
    uint8_t BLUE_REG_OFF_H = 0x00;

    // Register for led strip 1
    if(stripID == 1)
    {
        CW_REG_ON_L     = LED3_ON_L;    //= LED6_ON_L;
        CW_REG_ON_H     = LED3_ON_H;    //= LED6_ON_H;
        CW_REG_OFF_L    = LED3_OFF_L;   //= LED6_OFF_L;
        CW_REG_OFF_H    = LED3_OFF_H;   //= LED6_OFF_H;
        // -- ww                
        WW_REG_ON_L     = LED7_ON_L;    //= LED7_ON_L;
        WW_REG_ON_H     = LED7_ON_H;    //= LED7_ON_H;
        WW_REG_OFF_L    = LED7_OFF_L;   //= LED7_OFF_L;
        WW_REG_OFF_H    = LED7_OFF_H;   //= LED7_OFF_H;
        // -- RED              
        RED_REG_ON_L    = LED5_ON_L;    //= LED3_ON_L;
        RED_REG_ON_H    = LED5_ON_H;    //= LED3_ON_H;
        RED_REG_OFF_L   = LED5_OFF_L;   //= LED3_OFF_L;
        RED_REG_OFF_H   = LED5_OFF_H;   //= LED3_OFF_H;
        // -- GREEN             
        GREEN_REG_ON_L  = LED6_ON_L;    //= LED4_ON_L;
        GREEN_REG_ON_H  = LED6_ON_H;    //= LED4_ON_H;
        GREEN_REG_OFF_L = LED6_OFF_L;   //= LED4_OFF_L;
        GREEN_REG_OFF_H = LED6_OFF_H;   //= LED4_OFF_H;
        // -- BLUE             
        BLUE_REG_ON_L   = LED4_ON_L;    //= LED5_ON_L;
        BLUE_REG_ON_H   = LED4_ON_H;    //= LED5_ON_H;
        BLUE_REG_OFF_L  = LED4_OFF_L;   //= LED5_OFF_L;
        BLUE_REG_OFF_H  = LED4_OFF_H;   //= LED5_OFF_H; 
    }

    // Register for led strip 2
    if(stripID == 2)
    {
        CW_REG_ON_L     = LED8_ON_L;    //= LED1_ON_L;
        CW_REG_ON_H     = LED8_ON_H;    //= LED1_ON_H;
        CW_REG_OFF_L    = LED8_OFF_L;   //= LED1_OFF_L;
        CW_REG_OFF_H    = LED8_OFF_H;   //= LED1_OFF_H;
        // -- ww                
        WW_REG_ON_L     = LED12_ON_L;   //= LED12_ON_L;
        WW_REG_ON_H     = LED12_ON_H;   //= LED12_ON_H;
        WW_REG_OFF_L    = LED12_OFF_L;  //= LED12_OFF_L;
        WW_REG_OFF_H    = LED12_OFF_H;  //= LED12_OFF_H;
        // -- RED               
        RED_REG_ON_L    = LED10_ON_L;    //= LED8_ON_L;
        RED_REG_ON_H    = LED10_ON_H;    //= LED8_ON_H;
        RED_REG_OFF_L   = LED10_OFF_L;   //= LED8_OFF_L;
        RED_REG_OFF_H   = LED10_OFF_H;   //= LED8_OFF_H;
        // -- GREEN             
        GREEN_REG_ON_L  = LED11_ON_L;   //= LED9_ON_L;
        GREEN_REG_ON_H  = LED11_ON_H;   //= LED9_ON_H;
        GREEN_REG_OFF_L = LED11_OFF_L;  //= LED9_OFF_L;
        GREEN_REG_OFF_H = LED11_OFF_H;  //= LED9_OFF_H;
        // -- BLUE              
        BLUE_REG_ON_L   = LED9_ON_L;    //= LED10_ON_L;
        BLUE_REG_ON_H   = LED9_ON_H;    //= LED10_ON_H;
        BLUE_REG_OFF_L  = LED9_OFF_L;   //= LED10_OFF_L;
        BLUE_REG_OFF_H  = LED9_OFF_H;   //= LED10_OFF_H; 
    }
    // Clear all 

    // Effect State
    switch(effect)
    {

        // ---- Effect List
        case LEDEffect::None:
            
            if(power)
            {
                Serial.print("Test");
                // Red
                UpdateLEDChannel(   i2cAddress,
                                    RED_REG_ON_L,
                                    RED_REG_ON_H,
                                    RED_REG_OFF_L,
                                    RED_REG_OFF_H,
                                    red,
                                    brightness);
                // Green
                UpdateLEDChannel(   i2cAddress,
                                    GREEN_REG_ON_L,
                                    GREEN_REG_ON_H,
                                    GREEN_REG_OFF_L,
                                    GREEN_REG_OFF_H,
                                    green,
                                    brightness);
                // Blue
                UpdateLEDChannel(   i2cAddress,
                                    BLUE_REG_ON_L,
                                    BLUE_REG_ON_H,
                                    BLUE_REG_OFF_L,
                                    BLUE_REG_OFF_H,
                                    blue,
                                    brightness);

                // Cold White
                UpdateLEDChannel(   i2cAddress,
                                    CW_REG_ON_L,
                                    CW_REG_ON_H,
                                    CW_REG_OFF_L,
                                    CW_REG_OFF_H,
                                    cw,
                                    brightness);

                // Cold White
                UpdateLEDChannel(   i2cAddress,
                                    CW_REG_ON_L,
                                    CW_REG_ON_H,
                                    CW_REG_OFF_L,
                                    CW_REG_OFF_H,
                                    cw,
                                    brightness);
            }
            else
            {
                // Red
                UpdateLEDChannel(   i2cAddress,
                                    RED_REG_ON_L,
                                    RED_REG_ON_H,
                                    RED_REG_OFF_L,
                                    RED_REG_OFF_H,
                                    red,
                                    0);
                // Green
                UpdateLEDChannel(   i2cAddress,
                                    GREEN_REG_ON_L,
                                    GREEN_REG_ON_H,
                                    GREEN_REG_OFF_L,
                                    GREEN_REG_OFF_H,
                                    green,
                                    0);
                // Blue
                UpdateLEDChannel(   i2cAddress,
                                    BLUE_REG_ON_L,
                                    BLUE_REG_ON_H,
                                    BLUE_REG_OFF_L,
                                    BLUE_REG_OFF_H,
                                    blue,
                                    0);

                // Cold White
                UpdateLEDChannel(   i2cAddress,
                                    CW_REG_ON_L,
                                    CW_REG_ON_H,
                                    CW_REG_OFF_L,
                                    CW_REG_OFF_H,
                                    cw,
                                    0);
            }

            break;

        case LEDEffect::Alarm:
            /*
                Red flashing LEDs with cold white 
            */
            break;

        case LEDEffect::Music:
            /*
                Music blinking to music bpm
            */
            break;

        case LEDEffect::Sleep:
            /*
                Fades current color to warm orange (maybe warm white) and then fades to black
            */
            break;

        case LEDEffect::Weekend:
            /*
                Fades a warm orange to bright
            */
            break;

        case LEDEffect::RGB:
            /*
                Only red, green and blue LEDs
            */
            break;

        case LEDEffect::CW:
            /*
                Only cold white LEDs
            */
            break;

        case LEDEffect::WW:
            /*
                Only warm white LEDs
            */
            break;

        case LEDEffect::RGBCW:
            /*
                Only red, green, blue and cold white LEDs
            */
            break;

        case LEDEffect::RGBWW:
            /*
                Only red, green, blue and warm white LEDs
            */
            break;

        case LEDEffect::CWWW:
            /*
                Only cold white and warm white LEDs
            */
            break;   

    };
    
};


/**
 * Handels the control of a LED strip
 * @parameter None
 * @return Nones
 **/
void LedDriver::UpdateLEDChannel(   uint8_t i2cAddress,
                                    uint8_t REG_ON_L,
                                    uint8_t REG_ON_H,
                                    uint8_t REG_OFF_L,
                                    uint8_t REG_OFF_H,
                                    uint8_t colorValue, 
                                    uint8_t brightnessValue)
{
     /* 
         LED_ON_REG 12Bit 0000h - 0FFFh == 0 - 4095
         LED_OFF_REG 12Bit 0000h - 0FFFh == 0 - 4095

         Phase shift only for RGB (NO CW or WW)
     */
    float color = colorValue;
    float brightnessScale = (float)map(brightnessValue, 0, 255, 0, 100)/100.0;
    uint16_t data = (uint16_t)map((color * brightnessScale), 0, 255, 0, 4095);
    Serial.println("");
    Serial.print("Data     : ");
    Serial.println(data);
    Serial.print("LowByte  : ");
    PrintByte(lowByte(data));
    Serial.print("HighByte : ");
    PrintByte(highByte(data));

    i2c->write8(i2cAddress, REG_ON_L, lowByte(data));
    i2c->write8(i2cAddress, REG_ON_H, highByte(data));
    i2c->write8(i2cAddress, REG_OFF_L, 0b11111111);
    i2c->write8(i2cAddress, REG_OFF_H, 0b00001111);
};


/**
 * Prints all register values from the PCA9685PW
 * @parameter None
 * @return None
 **/
void LedDriver::PrintAllRegister()
{
    Serial.println("# ==== REGISTERS ==== #");
    for (int i = 0; i < 70; i++)
    {
      uint8_t reg_data = i2c->read8(0x40, i);    
      if (i < 16)
      {
        Serial.print("0");
      }
      Serial.print(i, HEX);
      Serial.print(" ");
      PrintByte(reg_data);
    }
    for (int i = 250; i < 256; i++)
    {
      uint8_t reg_data = i2c->read8(0x40, i);
      Serial.print(i, HEX);
      Serial.print(" ");
      PrintByte(reg_data);
    }
    Serial.println("# =================== #");
};


/**
 * Prints a byte with leading zeros 
 * @parameter Byte to print
 * @return None
 **/
void LedDriver::PrintByte(byte b)
{
    for (int i = 7; i >= 0; i--)
    {
      Serial.print(bitRead(b, i));
    }
    Serial.println("");
};


/**
 * Turns all LED Strips instant black
 * @parameter None
 * @return None
 **/
void LedDriver::AllBlack()
{
    // Strip 1
    HandleLEDStrip( 1, false, 0, 0, 0, 0, 0, 0, LEDEffect::None);

    // Strip 2
    HandleLEDStrip( 2, false, 0, 0, 0, 0, 0, 0, LEDEffect::None);
};