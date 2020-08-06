#include "../include/LedDriver.h"


/**
 * Constructor for the LedDriver class
 * 
 * @parameter i2cAddress    The i2c address of the pwm ic
 * @parameter *i2c          Pointer to the used instance of the I2C class
 * @parameter *network      Pointer to the used instace of the Network class
 * @parameter *pitReader    Pointer to the used instace of the PirReader class
 */
LedDriver::LedDriver(uint8_t i2cAddress, 
                     I2C *i2c, 
                     Network *network, 
                     PirReader *pirReader)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
    this->pirReader = pirReader;
};


/**
 * Does init stuff for the LedDriver component
 * 
 * @return True if successfull, false if not 
 */
bool LedDriver::Init()
{
    if (!init)
    {
        i2c->Init();

        // Set sleep bit to set prescaler
        i2c->write8(i2cAddress, MODE1, 0b00010000);

        // Set prescaler to 1526 hz
        i2c->write8(i2cAddress, PRE_SCALE, 0b00000011);

        // Reset sleep bit after prescaler set
        i2c->write8(i2cAddress, MODE1, 0b00000000);

        // Set Mode 2 register
        i2c->write8(i2cAddress, MODE2, 0b00000100);
        /*
            OUTDRV = 1
        */

        Serial.println("LED Driver initialized");
        init = true;
    }

    return init;
};


/**
 * Runs the LedDriver component. 
 */
void LedDriver::Run()
{
    if(!init)
    {
        return;
    }

    // -- Copy data from network 
    networkMotionData           = network->stNetworkMotionData;
    commandNetworkLEDStrip1Data = network->stNetworkLedStrip1Data;
    commandNetworkLEDStrip2Data = network->stNetworkLedStrip2Data;

   
    // ---- Handle led strips
    // -- Strip 1
    HandleLEDStrip( 1,
                    commandNetworkLEDStrip1Data);

    // -- Strip 2
    HandleLEDStrip( 2,
                    commandNetworkLEDStrip2Data);

};


/**
 * Handels the LED Strip
 *
 * @parameter stripID                       The strip id of the LED Strip which the transition gets applied to
 * @parameter motionData                    The motion data for the led strip
 * @parameter commandNetworkLEDStripData    The speed with which a brightnessValue gets faded to a new value
 **/

void LedDriver::HandleLEDStrip(uint8_t stripID,
                               NetworkLEDStripData commandNetworkLEDStripData)
{
    // StripID needs to be given else return
    if(stripID != 1 && stripID != 2)
    {
        return;
    }

    // ---- Control section
    /*
        Controls how the LED Strip behaves
    */

    
    // ---- Effect change section
    /*
        Checks if the effect state changed and performs a transition
    */


    // ---- Effect call section
    /*
        After effect transition is finished calls the new effect
    */
    switch(commandNetworkLEDStripData.effect)
    {

        case LEDEffect::None:
            /*
                Normal mode with no restrictions 
            */
           
            if(commandNetworkLEDStripData.power)
            {
                FadeToColor(stripID,
                            commandNetworkLEDStripData);
            }
            else
            {
                FadeToBlack(stripID);
            }

            break;


        case LEDEffect::Alarm:
            /*
                Red puls light 
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
            // Disable CW and WW

            break;


        case LEDEffect::CW:
            /*
                Only cold white LEDs
            */
            // Disable RGB and WW

            break;


        case LEDEffect::WW:
            /*
                Only warm white LEDs
            */
            // Disable RGB and CW

            break;


        case LEDEffect::RGBCW:
            /*
                Only red, green, blue and cold white LEDs
            */
            // Disable WW

            break;


        case LEDEffect::RGBWW:
            /*
                Only red, green, blue and warm white LEDs
            */
            // Disable CW

            break;


        case LEDEffect::CWWW:
            /*
                Only cold white and warm white LEDs
            */
            // Disable RGB

            break;   

    };
    
};


bool LedDriver::FadeToColor(uint8_t stripID,
                            NetworkLEDStripData commandNetworkLEDStripData)
{


};


bool LedDriver::FadeToColor(uint8_t stripID,
                            HighLevelLEDStripData commandHighLevelLEDStripData)
{


};


bool LedDriver::FadeToColor(uint8_t stripID,
                            LowLevelLEDStripData commandLowLevelLEDStripData)
{


};


bool LedDriver::FadeToBlack(uint8_t stripID)
{


};


/**
 * Writes a color value to the specified register with phase shift
 * 
 * @parameter REG               The LED Color channel registers to update with new color and brightness
 * @parameter phaseShift        The phase shift value to apply to the given LED channel
 * @parameter colorValue        The color value of the given LED channel
 * @parameter brightnessValue   The brightness of the given LED channel
 **/
void LedDriver::UpdateLEDChannel(LEDColorReg REG,
                                 uint16_t phaseShift,
                                 uint8_t colorValue, 
                                 uint16_t brightnessValue)
{
    /* 
        LED_ON => Value at which the LED is ON
        LED_ON_REG 12Bit 0000h - 0FFFh == 0 - 4095
            REG_ON_L 0-7Bit 00h - ffh
            REG_ON_H 0-3Bit 00h - 0fh

        LED_OFF => Value at which the LED is OFF
        LED_OFF_REG 12Bit 0000h - 0FFFh == 0 - 4095
            REG_OFF_L 0-7Bit 00h - ffh
            REG_OFF_H 0-3Bit 00h - 0fh
    */
    
    uint16_t data = 0;

    // Zero Devision check
    if(brightnessValue == 0 || colorValue == 0)
    {
        data = 0;
    }
    else
    {
        data = (uint16_t)(((double)(colorValue + 1) * 16.0)* ((double)brightnessValue / 4095.0));
    }

    // Bound Check
    if(data >= 4096)
    {
        data = 4095;
    }

    // LED_ON_REG
    uint16_t ON_REG = phaseShift;
    i2c->write8(i2cAddress, REG.ON_L, lowByte(ON_REG));
    i2c->write8(i2cAddress, REG.ON_H, highByte(ON_REG));

    // LED_OFF_REG
    uint16_t OFF_REG = 0;
    if(data + phaseShift <= 4095.0)
    {
        OFF_REG = data + phaseShift;
    }
    else
    {
        OFF_REG = data + phaseShift - 4096;
    }
    i2c->write8(i2cAddress, REG.OFF_L, lowByte(OFF_REG));
    i2c->write8(i2cAddress, REG.OFF_H, highByte(OFF_REG));

};


/**
 * Returns a pointer to the low level led strip data of the coresponding stripID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return Pointer to the LowLevelLEDStripData of the given stripID
 */
LowLevelLEDStripData* LedDriver::getLowLevelLEDStripDataOfStrip(uint8_t stripID)
{

    // LED Strip 1
    if(stripID == 1)
    {
        return &stateLowLevelLEDStrip1Data;
    }

    // LED Strip 2
    if(stripID == 2)
    {
        return &stateLowLevelLEDStrip2Data;
    }

};


/**
 * Returns all color channel registers of the given LED strip ID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return LEDStripColorReg with all color channel register
 */
LEDStripColorReg LedDriver::getColorRegForLEDStrip(uint8_t stripID)
{
    // LED Strip 1
    if(stripID == 1)
    {
        LEDStripColorReg STRIP;

        // CW
        STRIP.CW_REG.ON_L       = LED3_ON_L;    //= LED6_ON_L;
        STRIP.CW_REG.ON_H       = LED3_ON_H;    //= LED6_ON_H;
        STRIP.CW_REG.OFF_L      = LED3_OFF_L;   //= LED6_OFF_L;
        STRIP.CW_REG.OFF_H      = LED3_OFF_H;   //= LED6_OFF_H;

        // WW
        STRIP.WW_REG.ON_L       = LED7_ON_L;    //= LED7_ON_L;
        STRIP.WW_REG.ON_H       = LED7_ON_H;    //= LED7_ON_H;
        STRIP.WW_REG.OFF_L      = LED7_OFF_L;   //= LED7_OFF_L;
        STRIP.WW_REG.OFF_H      = LED7_OFF_H;   //= LED7_OFF_H;

        // RED
        STRIP.RED_REG.ON_L      = LED5_ON_L;    //= LED3_ON_L;
        STRIP.RED_REG.ON_H      = LED5_ON_H;    //= LED3_ON_H;
        STRIP.RED_REG.OFF_L     = LED5_OFF_L;   //= LED3_OFF_L;
        STRIP.RED_REG.OFF_H     = LED5_OFF_H;   //= LED3_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L    = LED6_ON_L;    //= LED4_ON_L;
        STRIP.GREEN_REG.ON_H    = LED6_ON_H;    //= LED4_ON_H;
        STRIP.GREEN_REG.OFF_L   = LED6_OFF_L;   //= LED4_OFF_L;
        STRIP.GREEN_REG.OFF_H   = LED6_OFF_H;   //= LED4_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L     = LED4_ON_L;    //= LED5_ON_L;
        STRIP.BLUE_REG.ON_H     = LED4_ON_H;    //= LED5_ON_H;
        STRIP.BLUE_REG.OFF_L    = LED4_OFF_L;   //= LED5_OFF_L;
        STRIP.BLUE_REG.OFF_H    = LED4_OFF_H;   //= LED5_OFF_H;
         
        return STRIP;
    }

    // LED Strip 2
    if(stripID == 2)
    {
        LEDStripColorReg STRIP;

        // CW
        STRIP.CW_REG.ON_L       = LED8_ON_L;    //= LED1_ON_L;
        STRIP.CW_REG.ON_H       = LED8_ON_H;    //= LED1_ON_H;
        STRIP.CW_REG.OFF_L      = LED8_OFF_L;   //= LED1_OFF_L;
        STRIP.CW_REG.OFF_H      = LED8_OFF_H;   //= LED1_OFF_H;

        // WW
        STRIP.WW_REG.ON_L       = LED12_ON_L;   //= LED12_ON_L;
        STRIP.WW_REG.ON_H       = LED12_ON_H;   //= LED12_ON_H;
        STRIP.WW_REG.OFF_L      = LED12_OFF_L;  //= LED12_OFF_L;
        STRIP.WW_REG.OFF_H      = LED12_OFF_H;  //= LED12_OFF_H;

        // RED
        STRIP.RED_REG.ON_L      = LED10_ON_L;    //= LED8_ON_L;
        STRIP.RED_REG.ON_H      = LED10_ON_H;    //= LED8_ON_H;
        STRIP.RED_REG.OFF_L     = LED10_OFF_L;   //= LED8_OFF_L;
        STRIP.RED_REG.OFF_H     = LED10_OFF_H;   //= LED8_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L    = LED11_ON_L;   //= LED9_ON_L;
        STRIP.GREEN_REG.ON_H    = LED11_ON_H;   //= LED9_ON_H;
        STRIP.GREEN_REG.OFF_L   = LED11_OFF_L;  //= LED9_OFF_L;
        STRIP.GREEN_REG.OFF_H   = LED11_OFF_H;  //= LED9_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L     = LED9_ON_L;    //= LED10_ON_L;
        STRIP.BLUE_REG.ON_H     = LED9_ON_H;    //= LED10_ON_H;
        STRIP.BLUE_REG.OFF_L    = LED9_OFF_L;   //= LED10_OFF_L;
        STRIP.BLUE_REG.OFF_H    = LED9_OFF_H;   //= LED10_OFF_H;

        return STRIP;
    }
};


/**
 * Prints all used register values from the PCA9685PW
 **/
void LedDriver::PrintAllRegister()
{
    Serial.println("# ==== PCA9685PW REGISTERS ==== #");
    for (int i = 0; i < 70; i++)
    {
      uint8_t reg_data = i2c->read8(i2cAddress, i);    
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
      uint8_t reg_data = i2c->read8(i2cAddress, i);
      Serial.print(i, HEX);
      Serial.print(" ");
      PrintByte(reg_data);
    }
    Serial.println("# ============================= #");
    Serial.println("");
};


/**
 * Prints a byte with leading zeros 
 * 
 * @parameter byte  The byte to print in binary
 **/
void LedDriver::PrintByte(byte b)
{
    for (int i = 7; i >= 0; i--)
    {
      Serial.print(bitRead(b, i));
    }
    Serial.println("");
};
                            