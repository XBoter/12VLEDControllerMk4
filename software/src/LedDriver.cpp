#include "../include/LedDriver.h"
#include "../Main.h"


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

        // Set default values
        defaultHighLevelLEDStripData.colorFadeTime          = 1000; // Milliseconds
        defaultHighLevelLEDStripData.colorFadeCurve         = FadeCurve::EaseInOut;
        defaultHighLevelLEDStripData.brightnessFadeTime     = 800; // Milliseconds
        defaultHighLevelLEDStripData.brightnessFadeCurve    = FadeCurve::Linear;

        network->stNetworkLedStrip1Data.ledStripData.colorFadeTime         = defaultHighLevelLEDStripData.colorFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve        = defaultHighLevelLEDStripData.colorFadeCurve;
        network->stNetworkLedStrip1Data.ledStripData.brightnessFadeTime    = defaultHighLevelLEDStripData.brightnessFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve   = defaultHighLevelLEDStripData.brightnessFadeCurve;

        network->stNetworkLedStrip2Data.ledStripData.colorFadeTime         = defaultHighLevelLEDStripData.colorFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve        = defaultHighLevelLEDStripData.colorFadeCurve;
        network->stNetworkLedStrip2Data.ledStripData.brightnessFadeTime    = defaultHighLevelLEDStripData.brightnessFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve   = defaultHighLevelLEDStripData.brightnessFadeCurve;

        // Calc refresh rate
        intervalRefreshRate = (unsigned long)(1000.0 / LED_STRIP_REFRESH_RATE);
        Serial.print("LED Strip get refreshed every '");
        Serial.print(intervalRefreshRate);
        Serial.println("' Milliseconds");

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

    // Refersh LED Strip data every x seconds => Needed for time based color fade
    unsigned long currentMillisRefreshRate = millis();
    if (currentMillisRefreshRate - previousMillisRefreshRate >= intervalRefreshRate) {
        //Serial.print("Update got called after '");
        //Serial.print(currentMillisRefreshRate - previousMillisRefreshRate);
        //Serial.println("' Milliseconds");
        previousMillisRefreshRate = currentMillisRefreshRate;

        // ---- Handle led strips
        // -- Strip 1
        HandleLEDStrip(1,
                       commandNetworkLEDStrip1Data);
        // -- Strip 2
        HandleLEDStrip(2,
                       commandNetworkLEDStrip2Data);

        // ---- Update LED strip
        // -- Strip 1
        UpdateLEDStrip(1);
        // -- Strip 2
        UpdateLEDStrip(2);
    }

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
                FadeToBlack(stripID,
                            commandNetworkLEDStripData);
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
    // Add missing default values for NetworkLEDStripData
    commandNetworkLEDStripData.ledStripData.colorFadeTime        = defaultHighLevelLEDStripData.colorFadeTime;
    commandNetworkLEDStripData.ledStripData.colorFadeCurve       = defaultHighLevelLEDStripData.colorFadeCurve;
    commandNetworkLEDStripData.ledStripData.brightnessFadeTime   = defaultHighLevelLEDStripData.brightnessFadeTime;
    commandNetworkLEDStripData.ledStripData.brightnessFadeCurve  = defaultHighLevelLEDStripData.brightnessFadeCurve;

    // Call high level fade
    bool fadeFinished = FadeToColor(stripID,
                                    commandNetworkLEDStripData.ledStripData);
    return fadeFinished;
};


bool LedDriver::FadeToColor(uint8_t stripID,
                            HighLevelLEDStripData commandHighLevelLEDStripData)
{

    // Create Low Level data
    LowLevelLEDStripData lowLevelLEDStripData;
    // ---- RED
    // -- Color
    lowLevelLEDStripData.redColorValue            = commandHighLevelLEDStripData.redColorValue;
    lowLevelLEDStripData.redColorFadeTime         = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.redColorFadeCurve        = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.redBrightnessValue       = commandHighLevelLEDStripData.brightnessValue;
    lowLevelLEDStripData.redBrightnessFadeTime    = commandHighLevelLEDStripData.brightnessFadeTime;
    lowLevelLEDStripData.redBrightnessFadeCurve   = commandHighLevelLEDStripData.brightnessFadeCurve;

    // ---- GREEN
    // -- Color
    lowLevelLEDStripData.greenColorValue          = commandHighLevelLEDStripData.greenColorValue;
    lowLevelLEDStripData.greenColorFadeTime       = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.greenColorFadeCurve      = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.greenBrightnessValue     = commandHighLevelLEDStripData.brightnessValue;
    lowLevelLEDStripData.greenBrightnessFadeTime  = commandHighLevelLEDStripData.brightnessFadeTime;
    lowLevelLEDStripData.greenBrightnessFadeCurve = commandHighLevelLEDStripData.brightnessFadeCurve;
    
    // ---- BLUE
    // -- Color
    lowLevelLEDStripData.blueColorValue           = commandHighLevelLEDStripData.blueColorValue;
    lowLevelLEDStripData.blueColorFadeTime        = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.blueColorFadeCurve       = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.blueBrightnessValue      = commandHighLevelLEDStripData.brightnessValue;
    lowLevelLEDStripData.blueBrightnessFadeTime   = commandHighLevelLEDStripData.brightnessFadeTime;
    lowLevelLEDStripData.blueBrightnessFadeCurve  = commandHighLevelLEDStripData.brightnessFadeCurve;

    // ---- CW
    // -- Color
    lowLevelLEDStripData.cwColorValue             = commandHighLevelLEDStripData.cwColorValue;
    lowLevelLEDStripData.cwColorFadeTime          = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.cwColorFadeCurve         = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.cwBrightnessValue        = commandHighLevelLEDStripData.brightnessValue;
    lowLevelLEDStripData.cwBrightnessFadeTime     = commandHighLevelLEDStripData.brightnessFadeTime;
    lowLevelLEDStripData.cwBrightnessFadeCurve    = commandHighLevelLEDStripData.brightnessFadeCurve;

    // ---- WW
    // -- Color
    lowLevelLEDStripData.wwColorValue             = commandHighLevelLEDStripData.wwColorValue;
    lowLevelLEDStripData.wwColorFadeTime          = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.wwColorFadeCurve         = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.wwBrightnessValue        = commandHighLevelLEDStripData.brightnessValue;
    lowLevelLEDStripData.wwBrightnessFadeTime     = commandHighLevelLEDStripData.brightnessFadeTime;
    lowLevelLEDStripData.wwBrightnessFadeCurve    = commandHighLevelLEDStripData.brightnessFadeCurve;

    // Call low level fade
    bool fadeFinished = FadeToColor(stripID,
                                    lowLevelLEDStripData);
    return fadeFinished;
};


bool LedDriver::FadeToColor(uint8_t stripID,
                            LowLevelLEDStripData commandLowLevelLEDStripData)
{

    unsigned long curMillis = previousMillisRefreshRate;

    // Get current data of strip
    LEDStripData* ptrCurrentLEDStripData = getCurrentLEDStripData(stripID);
    LEDStripData* ptrPreviousLEDStripData = getPreviousLEDStripData(stripID);

    // ---- RED
    // -- Color
    if(ptrCurrentLEDStripData->redColorValue != commandLowLevelLEDStripData.redColorValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.redColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisRedColorFade) / (double)commandLowLevelLEDStripData.redColorFadeTime;
        }
        ptrCurrentLEDStripData->redColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.redColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevRedColorValue,
                                                                       commandLowLevelLEDStripData.redColorValue);
        if(ptrCurrentLEDStripData->redColorValue == commandLowLevelLEDStripData.redColorValue)
        {
            ptrCurrentLEDStripData->prevRedColorValue = ptrCurrentLEDStripData->redColorValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisRedColorFade = curMillis;
    }
    // -- Brightness
    if(ptrCurrentLEDStripData->redBrightnessValue != commandLowLevelLEDStripData.redBrightnessValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.redBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisRedBrightnessFade) / (double)commandLowLevelLEDStripData.redBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->redBrightnessValue = getCurveValue(commandLowLevelLEDStripData.redBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevRedBrightnessValue,
                                                                   commandLowLevelLEDStripData.redBrightnessValue);
 
        if(ptrCurrentLEDStripData->redBrightnessValue == commandLowLevelLEDStripData.redBrightnessValue)
        {
            ptrCurrentLEDStripData->prevRedBrightnessValue = ptrCurrentLEDStripData->redBrightnessValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisRedBrightnessFade = curMillis;
    }

    // ---- GREEN
    // -- Color
    if(ptrCurrentLEDStripData->greenColorValue != commandLowLevelLEDStripData.greenColorValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.greenColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisGreenColorFade) / (double)commandLowLevelLEDStripData.greenColorFadeTime;
        }
        ptrCurrentLEDStripData->greenColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.greenColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevGreenColorValue,
                                                                       commandLowLevelLEDStripData.greenColorValue);
        if(ptrCurrentLEDStripData->greenColorValue == commandLowLevelLEDStripData.greenColorValue)
        {
            ptrCurrentLEDStripData->prevGreenColorValue = ptrCurrentLEDStripData->greenColorValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisGreenColorFade = curMillis;
    }
    // -- Brightness
    if(ptrCurrentLEDStripData->greenBrightnessValue != commandLowLevelLEDStripData.greenBrightnessValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.greenBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisGreenBrightnessFade) / (double)commandLowLevelLEDStripData.greenBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->greenBrightnessValue = getCurveValue(commandLowLevelLEDStripData.greenBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevGreenBrightnessValue,
                                                                   commandLowLevelLEDStripData.greenBrightnessValue);

        if(ptrCurrentLEDStripData->greenBrightnessValue == commandLowLevelLEDStripData.greenBrightnessValue)
        {
            ptrCurrentLEDStripData->prevGreenBrightnessValue = ptrCurrentLEDStripData->redBrightnessValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisGreenBrightnessFade = curMillis;
    }

    // ---- BLUE
    // -- Color
    if(ptrCurrentLEDStripData->blueColorValue != commandLowLevelLEDStripData.blueColorValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.blueColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisBlueColorFade) / (double)commandLowLevelLEDStripData.blueColorFadeTime;
        }
        ptrCurrentLEDStripData->blueColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.blueColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevBlueColorValue,
                                                                       commandLowLevelLEDStripData.blueColorValue);
        if(ptrCurrentLEDStripData->blueColorValue == commandLowLevelLEDStripData.blueColorValue)
        {
            ptrCurrentLEDStripData->prevBlueColorValue = ptrCurrentLEDStripData->blueColorValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisBlueColorFade = curMillis;
    }
    // -- Brightness
    if(ptrCurrentLEDStripData->blueBrightnessValue != commandLowLevelLEDStripData.blueBrightnessValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.blueBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisBlueBrightnessFade) / (double)commandLowLevelLEDStripData.blueBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->blueBrightnessValue = getCurveValue(commandLowLevelLEDStripData.blueBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevBlueBrightnessValue,
                                                                   commandLowLevelLEDStripData.blueBrightnessValue);

        if(ptrCurrentLEDStripData->blueBrightnessValue == commandLowLevelLEDStripData.blueBrightnessValue)
        {
            ptrCurrentLEDStripData->prevBlueBrightnessValue = ptrCurrentLEDStripData->blueBrightnessValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisBlueBrightnessFade = curMillis;
    }

    // ---- CW
    // -- Color
    if(ptrCurrentLEDStripData->cwColorValue != commandLowLevelLEDStripData.cwColorValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.cwColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisCwColorFade) / (double)commandLowLevelLEDStripData.cwColorFadeTime;
        }
        ptrCurrentLEDStripData->cwColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.cwColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevCwColorValue,
                                                                       commandLowLevelLEDStripData.cwColorValue);
        if(ptrCurrentLEDStripData->cwColorValue == commandLowLevelLEDStripData.cwColorValue)
        {
            ptrCurrentLEDStripData->prevCwColorValue = ptrCurrentLEDStripData->cwColorValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisCwColorFade = curMillis;
    }
    // -- Brightness
    if(ptrCurrentLEDStripData->cwBrightnessValue != commandLowLevelLEDStripData.cwBrightnessValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.cwBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisCwBrightnessFade) / (double)commandLowLevelLEDStripData.cwBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->cwBrightnessValue = getCurveValue(commandLowLevelLEDStripData.cwBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevCwBrightnessValue,
                                                                   commandLowLevelLEDStripData.cwBrightnessValue);

        if(ptrCurrentLEDStripData->cwBrightnessValue == commandLowLevelLEDStripData.cwBrightnessValue)
        {
            ptrCurrentLEDStripData->prevCwBrightnessValue = ptrCurrentLEDStripData->cwBrightnessValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisCwBrightnessFade = curMillis;
    }

    // ---- WW
    // -- Color
    if(ptrCurrentLEDStripData->wwColorValue != commandLowLevelLEDStripData.wwColorValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.wwColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisWwColorFade) / (double)commandLowLevelLEDStripData.wwColorFadeTime;
        }
        ptrCurrentLEDStripData->wwColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.wwColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevWwColorValue,
                                                                       commandLowLevelLEDStripData.wwColorValue);
        if(ptrCurrentLEDStripData->wwColorValue == commandLowLevelLEDStripData.wwColorValue)
        {
            ptrCurrentLEDStripData->prevWwColorValue = ptrCurrentLEDStripData->wwColorValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisWwColorFade = curMillis;
    }
    // -- Brightness
    if(ptrCurrentLEDStripData->wwBrightnessValue != commandLowLevelLEDStripData.wwBrightnessValue)
    {
        double percent = 0.0;
        if(commandLowLevelLEDStripData.wwBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisWwBrightnessFade) / (double)commandLowLevelLEDStripData.wwBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->wwBrightnessValue = getCurveValue(commandLowLevelLEDStripData.wwBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevWwBrightnessValue,
                                                                   commandLowLevelLEDStripData.wwBrightnessValue);

        if(ptrCurrentLEDStripData->wwBrightnessValue == commandLowLevelLEDStripData.wwBrightnessValue)
        {
            ptrCurrentLEDStripData->prevWwBrightnessValue = ptrCurrentLEDStripData->wwBrightnessValue;
        }
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisWwBrightnessFade = curMillis;
    }

};


bool LedDriver::FadeToBlack(uint8_t stripID,
                            NetworkLEDStripData commandNetworkLEDStripData)
{

    // Add missing default values for NetworkLEDStripData
    commandNetworkLEDStripData.ledStripData.colorFadeTime        = defaultHighLevelLEDStripData.colorFadeTime;
    commandNetworkLEDStripData.ledStripData.colorFadeCurve       = defaultHighLevelLEDStripData.colorFadeCurve;
    commandNetworkLEDStripData.ledStripData.brightnessFadeTime   = defaultHighLevelLEDStripData.brightnessFadeTime;
    commandNetworkLEDStripData.ledStripData.brightnessFadeCurve  = defaultHighLevelLEDStripData.brightnessFadeCurve;


    // Call high level fade
    bool fadeFinished = FadeToBlack(stripID,
                                    commandNetworkLEDStripData.ledStripData);
    return fadeFinished;
};


bool LedDriver::FadeToBlack(uint8_t stripID,
                            HighLevelLEDStripData commandHighLevelLEDStripData)
{

    // Brightness 0%
    commandHighLevelLEDStripData.brightnessValue = 0;


    // Call high level fade
    bool fadeFinished = FadeToColor(stripID,
                                    commandHighLevelLEDStripData);
    return fadeFinished;
};



void LedDriver::UpdateLEDStrip(uint8_t stripID)
{
    // Get current data of strip
    LEDStripData* ptrCurrentLEDStripData = getCurrentLEDStripData(stripID);
    LEDStripData* ptrPreviousLEDStripData = getPreviousLEDStripData(stripID);

    // Get led reg of strip
    LEDStripColorReg STRIP = getColorRegForLEDStrip(stripID);

    // Phase shift
    uint16_t phaseShiftRed      = 0;
    uint16_t phaseShiftCw       = 819;
    uint16_t phaseShiftGreen    = 1639;
    uint16_t phaseShiftWw       = 2459;
    uint16_t phaseShiftBlue     = 3279;

    // ---- Update each color channel
    // -- RED
    if( ptrCurrentLEDStripData->redColorValue            != ptrPreviousLEDStripData->redColorValue
        || ptrCurrentLEDStripData->redBrightnessValue    != ptrPreviousLEDStripData->redBrightnessValue)
    {
        UpdateLEDChannel(STRIP.RED_REG,
                         phaseShiftRed, 
                         ptrCurrentLEDStripData->redColorValue,
                         ptrCurrentLEDStripData->redBrightnessValue);

        ptrPreviousLEDStripData->redColorValue       = ptrCurrentLEDStripData->redColorValue;
        ptrPreviousLEDStripData->redBrightnessValue  = ptrCurrentLEDStripData->redBrightnessValue;
    }

    // -- GREEN
    if( ptrCurrentLEDStripData->greenColorValue            != ptrPreviousLEDStripData->greenColorValue
        || ptrCurrentLEDStripData->greenBrightnessValue    != ptrPreviousLEDStripData->greenBrightnessValue)
    {
        UpdateLEDChannel(STRIP.GREEN_REG,
                         phaseShiftGreen, 
                         ptrCurrentLEDStripData->greenColorValue,
                         ptrCurrentLEDStripData->greenBrightnessValue);

        ptrPreviousLEDStripData->greenColorValue       = ptrCurrentLEDStripData->greenColorValue;
        ptrPreviousLEDStripData->greenBrightnessValue  = ptrCurrentLEDStripData->greenBrightnessValue;
    }

    // -- BLUE
    if( ptrCurrentLEDStripData->blueColorValue            != ptrPreviousLEDStripData->blueColorValue
        || ptrCurrentLEDStripData->blueBrightnessValue    != ptrPreviousLEDStripData->blueBrightnessValue)
    {
        UpdateLEDChannel(STRIP.BLUE_REG,
                         phaseShiftBlue, 
                         ptrCurrentLEDStripData->blueColorValue,
                         ptrCurrentLEDStripData->blueBrightnessValue);

        ptrPreviousLEDStripData->blueColorValue       = ptrCurrentLEDStripData->blueColorValue;
        ptrPreviousLEDStripData->blueBrightnessValue  = ptrCurrentLEDStripData->blueBrightnessValue;
    }

    // -- CW
    if( ptrCurrentLEDStripData->cwColorValue            != ptrPreviousLEDStripData->cwColorValue
        || ptrCurrentLEDStripData->cwBrightnessValue    != ptrPreviousLEDStripData->cwBrightnessValue)
    {
        UpdateLEDChannel(STRIP.CW_REG,
                         phaseShiftCw, 
                         ptrCurrentLEDStripData->cwColorValue,
                         ptrCurrentLEDStripData->cwBrightnessValue);

        ptrPreviousLEDStripData->cwColorValue       = ptrCurrentLEDStripData->cwColorValue;
        ptrPreviousLEDStripData->cwBrightnessValue  = ptrCurrentLEDStripData->cwBrightnessValue;
    }

    // -- WW
    if( ptrCurrentLEDStripData->wwColorValue            != ptrPreviousLEDStripData->wwColorValue
        || ptrCurrentLEDStripData->wwBrightnessValue    != ptrPreviousLEDStripData->wwBrightnessValue)
    {
        UpdateLEDChannel(STRIP.WW_REG,
                         phaseShiftWw, 
                         ptrCurrentLEDStripData->wwColorValue,
                         ptrCurrentLEDStripData->wwBrightnessValue);

        ptrPreviousLEDStripData->wwColorValue       = ptrCurrentLEDStripData->wwColorValue;
        ptrPreviousLEDStripData->wwBrightnessValue  = ptrCurrentLEDStripData->wwBrightnessValue;
    }



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
 * Returns the value of the given curve at the given percent
 * 
 * @parameter curve     The used curve
 * @parameter percent   Percentage between start an end
 * @parameter start     The start value of the function
 * @parameter end       The end value of the function
 * 
 * @return The current value of the function at the given percent
 */
uint16_t LedDriver::getCurveValue(FadeCurve curve,
                                  double percent,
                                  int start,
                                  int end)
{
    // Bound check
    if(percent >= 1.0)
    {
        percent = 1.0;
    }
    if(percent <= 0.0)
    {
        percent = 0.0;
    }
    
    switch (curve)
    {
        case FadeCurve::None:
            return end;
            break;

        case FadeCurve::Linear:
            return linear(percent,
                          start,
                          end);
            break;

        case FadeCurve::EaseIn:
            return easeInQuart(percent,
                               start,
                               end);
            break;

        case FadeCurve::EaseOut:
            return easeOutQuart(percent,
                                start,
                                end);
            break;

        case FadeCurve::EaseInOut:
            return easeInOutQuart(percent,
                                  start,
                                  end);
            break;

        default:
            return end;
            break;
    }

};


/**
 * Returns a pointer to the current LEDStripData of the coresponding stripID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return Pointer to the current LEDStripData of the given stripID
 */
LEDStripData* LedDriver::getCurrentLEDStripData(uint8_t stripID)
{

    // LED Strip 1
    if(stripID == 1)
    {
        return &currentLEDStrip1Data;
    }

    // LED Strip 2
    if(stripID == 2)
    {
        return &currentLEDStrip2Data;
    }

};


/**
 * Returns a pointer to the previous LEDStripData of the coresponding stripID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return Pointer to the previous LEDStripData of the given stripID
 */
LEDStripData* LedDriver::getPreviousLEDStripData(uint8_t stripID)
{

    // LED Strip 1
    if(stripID == 1)
    {
        return &prevLEDStrip1Data;
    }

    // LED Strip 2
    if(stripID == 2)
    {
        return &prevLEDStrip2Data;
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
                            

/**
 * Linear function
 * 
 * @parameter percent   Percentage between start an end
 * @parameter start     The start value of the function
 * @parameter end       The end value of the function
 * 
 * @return The current value of the function at the given percent
 */
uint16_t LedDriver::linear(double percent,
                           int start,
                           int end)
{
    return start + (end - start) * percent;
};


/**
 * EaseInQuad function
 * 
 * @parameter percent   Percentage between start an end
 * @parameter start     The start value of the function
 * @parameter end       The end value of the function
 * 
 * @return The current value of the function at the given percent
 */
uint16_t LedDriver::easeInQuart(double percent,
                                int start,
                                int end)
{
    return start + (end - start) * pow(percent, 4);
};


/**
 * EaseOutQuad function
 * 
 * @parameter percent   Percentage between start an end
 * @parameter start     The start value of the function
 * @parameter end       The end value of the function
 * 
 * @return The current value of the function at the given percent
 */
uint16_t LedDriver::easeOutQuart(double percent,
                                 int start,
                                 int end)
{
    return start + (end - start) * (1.0 - pow(1.0 - percent, 4));
};


/**
 * EaseInOut function
 * 
 * @parameter percent   Percentage between start an end
 * @parameter start     The start value of the function
 * @parameter end       The end value of the function
 * 
 * @return The current value of the function at the given percent
 */
uint16_t LedDriver::easeInOutQuart(double percent,
                                   int start,
                                   int end)
{
    return start + (end - start) * (percent < 0.5 ? 8 * pow(percent, 4) : 1 - pow(-2 * percent + 2, 4) / 2);
};




