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

        // Turn all LEDs of 
        // TODO turn all leds of after init

        PrintAllRegister();

        // ---- Init LEDStripData (Needed so after bootup the values change with the one received from mqtt and get updated)
        // -- Strip 1
        prevDataStrip1.brightness   = 100;
        prevDataStrip1.red          = 10;
        prevDataStrip1.green        = 10;
        prevDataStrip1.blue         = 10;
        prevDataStrip1.cw           = 10;
        prevDataStrip1.ww           = 10;
        // -- Strip 2
        prevDataStrip2.brightness   = 100;
        prevDataStrip2.red          = 10;
        prevDataStrip2.green        = 10;
        prevDataStrip2.blue         = 10;
        prevDataStrip2.cw           = 10;
        prevDataStrip2.ww           = 10;

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

    // ---- Get data from network instance
    // -- Motion
    motionData = network->stMotionData;
    // -- Strip 1
    curDataStrip1 = network->stLedStrip1Data;
    // -- Strip 2
    curDataStrip2 = network->stLedStrip2Data;

   
    // ---- Handle led strips
    // -- Strip 1
    HandleLEDStrip( 1,
                    1,
                    50,
                    motionData,
                    curDataStrip1,
                    &prevDataStrip1);

    // -- Strip 2
    HandleLEDStrip( 2,
                    1,
                    50,
                    motionData,
                    curDataStrip2,
                    &prevDataStrip2);

};


/**
 * Handels the control of a LED strip
 * @parameter None
 * @return None
 **/
void LedDriver::HandleLEDStrip( uint8_t stripID,
                                uint8_t colorFadeSpeed,
                                uint8_t brightnessFadeSpeed,
                                MotionData motionData,
                                LEDStripData curDataStrip,
                                LEDStripData *prevDataStrip)
{
    // StripID needs to be given else return
    if(stripID != 1 && stripID != 2)
    {
        return;
    }

    // Effect State
    switch(curDataStrip.effect)
    {

        // ---- Effect List
        case LEDEffect::None:
             /*
                Normal mode with no restrictions 
            */
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
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
            // Disable CW and WW
            curDataStrip.cw = 0;
            curDataStrip.ww = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;

        case LEDEffect::CW:
            /*
                Only cold white LEDs
            */
            // Disable RGB and WW
            curDataStrip.red = 0;
            curDataStrip.green = 0;
            curDataStrip.blue = 0;
            curDataStrip.ww = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;

        case LEDEffect::WW:
            /*
                Only warm white LEDs
            */
            // Disable RGB and CW
            curDataStrip.red = 0;
            curDataStrip.green = 0;
            curDataStrip.blue = 0;
            curDataStrip.cw = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;

        case LEDEffect::RGBCW:
            /*
                Only red, green, blue and cold white LEDs
            */
           // Disable WW
            curDataStrip.ww = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;

        case LEDEffect::RGBWW:
            /*
                Only red, green, blue and warm white LEDs
            */
            // Disable CW
            curDataStrip.cw = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;

        case LEDEffect::CWWW:
            /*
                Only cold white and warm white LEDs
            */
            // Disable RGB
            curDataStrip.red = 0;
            curDataStrip.green = 0;
            curDataStrip.blue = 0;
            if(curDataStrip.power)
            {
                FadeToColor(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            else
            {
                FadeToBlack(stripID,
                            colorFadeSpeed,
                            brightnessFadeSpeed,
                            curDataStrip,
                            prevDataStrip);
            }
            break;   

    };
    
};


void LedDriver::FadeToBlack(uint8_t stripID,
                            uint8_t colorFadeSpeed,
                            uint8_t brightnessFadeSpeed,
                            LEDStripData curDataStrip,
                            LEDStripData *prevDataStrip)
{
    // StripID needs to be given else return
    if(stripID != 1 && stripID != 2)
    {
        return;
    }

    // Set brightness to zero
    curDataStrip.brightness = 0;

    FadeToColor(stripID,
                colorFadeSpeed,
                brightnessFadeSpeed,
                curDataStrip,
                prevDataStrip);

};


void LedDriver::FadeToColor(uint8_t stripID,
                            uint8_t colorFadeSpeed,
                            uint8_t brightnessFadeSpeed,
                            LEDStripData curDataStrip,
                            LEDStripData *prevDataStrip)
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

    // ---- Fade
    /*
        Only call register functions if new values need to be written
    */
    
    int _colorFadeSpeed = colorFadeSpeed;
    int _brightnessFadeSpeed = brightnessFadeSpeed;

    // -- Brightness
    int _curBrightness = curDataStrip.brightness;
    int _prevBrightness = prevDataStrip->brightness;
    bool brightnessChanged = false;

    if((_prevBrightness + _brightnessFadeSpeed) < _curBrightness)
    {
        prevDataStrip->brightness += _brightnessFadeSpeed;
        brightnessChanged = true;
    }
    if((_prevBrightness - _brightnessFadeSpeed) > _curBrightness)
    {
        prevDataStrip->brightness -= _brightnessFadeSpeed;
        brightnessChanged = true;
    }
    if(((_prevBrightness + _brightnessFadeSpeed) >= _curBrightness) 
        && ((_prevBrightness - _brightnessFadeSpeed) <= _curBrightness)
        && !brightnessChanged
        && prevDataStrip->brightness != curDataStrip.brightness)
    {
        prevDataStrip->brightness = curDataStrip.brightness;
        brightnessChanged = true;
    }

    // -- Red
    int _curRed = curDataStrip.red;
    int _prevRed = prevDataStrip->red;
    bool redChanged = false;

    if((_prevRed + _colorFadeSpeed) < _curRed)
    {
        prevDataStrip->red += _colorFadeSpeed;
        redChanged = true;
    }
    if((_prevRed - _colorFadeSpeed) > _curRed)
    {
        prevDataStrip->red -= _colorFadeSpeed;
        redChanged = true;
    }
    if(((_prevRed + _colorFadeSpeed) >= _curRed) 
        && ((_prevRed - _colorFadeSpeed) <= _curRed)
        && !redChanged
        && prevDataStrip->red != curDataStrip.red)
    {
        prevDataStrip->red = curDataStrip.red;
        redChanged = true;
    }
    if(redChanged || brightnessChanged)
    {
        UpdateLEDChannel(   i2cAddress,
                            RED_REG_ON_L,
                            RED_REG_ON_H,
                            RED_REG_OFF_L,
                            RED_REG_OFF_H,
                            prevDataStrip->red,
                            prevDataStrip->brightness);
    }

    // -- Green
    int _curGreen = curDataStrip.green;
    int _prevGreen = prevDataStrip->green;
    bool greenChanged = false;

    if((_prevGreen + _colorFadeSpeed) < _curGreen)
    {
        prevDataStrip->green += _colorFadeSpeed;
        greenChanged = true;
    }
    if((_prevGreen - _colorFadeSpeed) > _curGreen)
    {
        prevDataStrip->green -= _colorFadeSpeed;
        greenChanged = true;
    }
    if(((_prevGreen + _colorFadeSpeed) >= _curGreen) 
        && ((_prevGreen - _colorFadeSpeed) <= _curGreen)
        && !greenChanged
        && prevDataStrip->green != curDataStrip.green)
    {
        prevDataStrip->green = curDataStrip.green;
        greenChanged = true;
    }
    if(greenChanged || brightnessChanged)
    {
        UpdateLEDChannel(   i2cAddress,
                            GREEN_REG_ON_L,
                            GREEN_REG_ON_H,
                            GREEN_REG_OFF_L,
                            GREEN_REG_OFF_H,
                            prevDataStrip->green,
                            prevDataStrip->brightness);
    }

    // -- Blue
    int _curBlue = curDataStrip.blue;
    int _prevBlue = prevDataStrip->blue;
    bool blueChanged = false;

    if((_prevBlue + _colorFadeSpeed) < _curBlue)
    {
        prevDataStrip->blue += _colorFadeSpeed;
        blueChanged = true;
    }
    if((_prevBlue - _colorFadeSpeed) > _curBlue)
    {
        prevDataStrip->blue -= _colorFadeSpeed;
        blueChanged = true;
    }
    if(((_prevBlue + _colorFadeSpeed) >= _curBlue) 
        && ((_prevBlue - _colorFadeSpeed) <= _curBlue)
        && !blueChanged
        && prevDataStrip->blue != curDataStrip.blue)
    {
        prevDataStrip->blue = curDataStrip.blue;
        blueChanged = true;
    }
    if(blueChanged || brightnessChanged)
    {
        UpdateLEDChannel(   i2cAddress,
                            BLUE_REG_ON_L,
                            BLUE_REG_ON_H,
                            BLUE_REG_OFF_L,
                            BLUE_REG_OFF_H,
                            prevDataStrip->blue,
                            prevDataStrip->brightness);
    }


    // -- Cold White
    int _curCW = curDataStrip.cw;
    int _prevCW = prevDataStrip->cw;
    bool cwChanged = false;

    if((_prevCW + _colorFadeSpeed) < _curCW)
    {
        prevDataStrip->cw += _colorFadeSpeed;
        cwChanged = true;
    }
    if((_prevCW - _colorFadeSpeed) > _curCW)
    {
        prevDataStrip->cw -= _colorFadeSpeed;
        cwChanged = true;
    }
    if(((_prevCW + _colorFadeSpeed) >= _curCW) 
        && ((_prevCW - _colorFadeSpeed) <= _curCW)
        && !cwChanged
        && prevDataStrip->cw != curDataStrip.cw)
    {
        prevDataStrip->cw = curDataStrip.cw;
        cwChanged = true;
    }
    if(cwChanged || brightnessChanged)
    {
        UpdateLEDChannel(   i2cAddress,
                            CW_REG_ON_L,
                            CW_REG_ON_H,
                            CW_REG_OFF_L,
                            CW_REG_OFF_H,
                            prevDataStrip->cw,
                            prevDataStrip->brightness);
    }

    // -- Warm White
    int _curWW = curDataStrip.ww;
    int _prevWW = prevDataStrip->ww;
    bool wwChanged = false;

    if((_prevWW + _colorFadeSpeed) < _curWW)
    {
        prevDataStrip->ww += _colorFadeSpeed;
        wwChanged = true;
    }
    if((_prevWW - _colorFadeSpeed) > _curWW)
    {
        prevDataStrip->ww -= _colorFadeSpeed;
        wwChanged = true;
    }
    if(((_prevWW + _colorFadeSpeed) >= _curWW) 
        && ((_prevWW - _colorFadeSpeed) <= _curWW)
        && !wwChanged
        && prevDataStrip->ww != curDataStrip.ww)
    {
        prevDataStrip->ww = curDataStrip.ww;
        wwChanged = true;
    }
    if(wwChanged || brightnessChanged)
    {
        UpdateLEDChannel(   i2cAddress,
                            WW_REG_ON_L,
                            WW_REG_ON_H,
                            WW_REG_OFF_L,
                            WW_REG_OFF_H,
                            prevDataStrip->ww,
                            prevDataStrip->brightness);
    }

};



/**
 * Writes a color value to the specified register
 * @parameter i2cAddress: i2c address of the pca9685
 * @return Nones
 **/
void LedDriver::UpdateLEDChannel(   uint8_t i2cAddress,
                                    uint8_t REG_ON_L,
                                    uint8_t REG_ON_H,
                                    uint8_t REG_OFF_L,
                                    uint8_t REG_OFF_H,
                                    uint8_t colorValue, 
                                    uint16_t brightnessValue)
{
    /* 
        LED_ON_REG 12Bit 0000h - 0FFFh == 0 - 4095
        LED_OFF_REG 12Bit 0000h - 0FFFh == 0 - 4095
        Phase shift only for RGB (NO CW or WW)
    */

    double brightnessScale = (double)brightnessValue / 4095.0;
    uint16_t data = (uint16_t)(map(((double)colorValue * brightnessScale), 0, 255, 0, 4095));
    //Serial.println("");
    //Serial.print("Data     : ");
    //Serial.println(data);
    //Serial.print("LowByte  : ");
    //PrintByte(lowByte(data));
    //Serial.print("HighByte : ");
    //PrintByte(highByte(data));

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
