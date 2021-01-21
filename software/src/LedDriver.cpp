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

// # ================================================================ ================================================================ # //
// #                                                             INTERFACE                                                             # //
// # ================================================================ ================================================================ # //

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

        // ---- Default Values
        // -- Create default value template RGB
        defaultHighLevelLEDStripData.colorFadeTime = 1000; // Milliseconds
        defaultHighLevelLEDStripData.colorFadeCurve = FadeCurve::EaseInOut;
        defaultHighLevelLEDStripData.colorBrightnessFadeTime = 800; // Milliseconds
        defaultHighLevelLEDStripData.colorBrightnessFadeCurve = FadeCurve::EaseInOut;

        // -- Create default value template White
        defaultHighLevelLEDStripData.whiteTemperatureFadeTime = 1000; // Milliseconds
        defaultHighLevelLEDStripData.whiteTemperatureFadeCurve = FadeCurve::EaseInOut;
        defaultHighLevelLEDStripData.whiteBrightnessFadeTime = 800; // Milliseconds
        defaultHighLevelLEDStripData.whiteBrightnessFadeCurve = FadeCurve::EaseInOut;

        // -- Set default value template data
        /*
            FadeTimes and FadeCurves get not send over network yet so we use the default values
        */
        // -- RGB
        network->stNetworkLedStrip1Data.ledStripData.colorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
        network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;
        // -- White
        network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime = defaultHighLevelLEDStripData.whiteTemperatureFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve = defaultHighLevelLEDStripData.whiteTemperatureFadeCurve;
        network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime = defaultHighLevelLEDStripData.whiteBrightnessFadeTime;
        network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve = defaultHighLevelLEDStripData.whiteBrightnessFadeCurve;

        // -- RGB
        network->stNetworkLedStrip2Data.ledStripData.colorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
        network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;
        // -- White
        network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime = defaultHighLevelLEDStripData.whiteTemperatureFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve = defaultHighLevelLEDStripData.whiteTemperatureFadeCurve;
        network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime = defaultHighLevelLEDStripData.whiteBrightnessFadeTime;
        network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve = defaultHighLevelLEDStripData.whiteBrightnessFadeCurve;

        // ---- Calculate refresh rate data
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
    if (!init)
    {
        return;
    }

    // ---- Copy data from network to led driver
    networkMusicData = network->stNetworkMusicData;
    networkAlarmData = network->stNetworkAlarmData;
    networkLEDStrip1Data = network->stNetworkLedStrip1Data;
    networkLEDStrip2Data = network->stNetworkLedStrip2Data;

    // ---- Multi LED Strip
    // -- Set multi led strip power
    if (networkLEDStrip1Data.power || networkLEDStrip2Data.power)
    {
        multiLEDStripPower = true;
    }
    else
    {
        multiLEDStripPower = false;
    }

    // ---- Main call of LED logic
    // Refersh LED Strip data every x seconds => Needed for time based color fade
    unsigned long currentMillisRefreshRate = millis();
    if (currentMillisRefreshRate - previousMillisRefreshRate >= intervalRefreshRate)
    {
        //Serial.print("Update got called after '");
        //Serial.print(currentMillisRefreshRate - previousMillisRefreshRate);
        //Serial.println("' Milliseconds");
        previousMillisRefreshRate = currentMillisRefreshRate;
        refreshRateCounter++;

        // -- Handle multi LED strip effects
        HandleMultiLEDStripEffects();

        // ---- Update LED strip
        // -- Strip 1
        UpdateLEDStrip(1);
        // -- Strip 2
        UpdateLEDStrip(2);
    }
};

// # ================================================================ ================================================================ # //
// #                                                               EFFECTS                                                             # //
// # ================================================================ ================================================================ # //

/**
 * Handels the display of multi LED strip effects
 */
void LedDriver::HandleMultiLEDStripEffects()
{
    // -- Data
    HighLevelLEDStripData highLevelLEDStripData;
    LowLevelLEDStripData lowLevelLEDStripData;

    // ---- Control use of multi LED strip effects
    if (networkAlarmData.power || !alarmFinished)
    {
        curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Alarm;
    }
    else
    {
        if (network->parameter_master_present)
        {
            if (networkMusicData.power)
            {
                curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Music;
            }
            else
            {
                if (multiLEDStripPower)
                {
                    curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Power;
                }
                else
                {
                    if (!network->enable_pc_present)
                    {
                        if (network->parameter_pc_present)
                        {
                            curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Idle;
                        }
                        else
                        {
                            if (pirReader->motionDetected && network->stNetworkMotionData.motionDetectionEnabled && network->parameter_sun)
                            {
                                curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::MotionDetected;
                            }
                            else
                            {
                                curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Idle;
                            }
                        }
                    }
                    else
                    {
                        if (pirReader->motionDetected && network->stNetworkMotionData.motionDetectionEnabled && network->parameter_sun)
                        {
                            curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::MotionDetected;
                        }
                        else
                        {
                            curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Idle;
                        }
                    }
                }
            }
        }
        else
        {
            curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::NoMasterPresent;
        }
    }

    HandleSingleLEDStripEffects(1, networkLEDStrip1Data);

    HandleSingleLEDStripEffects(2, networkLEDStrip2Data);
    return;

    // Check for effect change and reset transitionState
    if (curMultiLEDStripEffectData.multiLEDEffect != prevMultiLEDStripEffectData.multiLEDEffect)
    {
        curMultiLEDStripEffectData.transitionState = 0;
        curMultiLEDStripEffectData.subTransitionState = 0;
        curMultiLEDStripEffectData.counter = 0;
    }

    // -- Current multi LED strip effect
    switch (curMultiLEDStripEffectData.multiLEDEffect)
    {

    case MultiLEDEffect::NoMasterPresent:
        /*
                Fades all to black
            */
        ResetSingleEffectData();
        highLevelLEDStripData = getDefaultHigh();

        highLevelLEDStripData.blueColorValue = 255;
        highLevelLEDStripData.colorBrightnessValue = 4096;

        switch (curMultiLEDStripEffectData.transitionState)
        {
        // Check for Mode change
        case 0:
            if (curMultiLEDStripEffectData.multiLEDEffect != prevMultiLEDStripEffectData.multiLEDEffect)
            {
                prevMultiLEDStripEffectData.multiLEDEffect = curMultiLEDStripEffectData.multiLEDEffect;
                curMultiLEDStripEffectData.transitionState = 10;
            }
            else
            {
                // Fade all to black
                FadeToBlack(highLevelLEDStripData);
            }
            break;
        // Fade black
        case 10:
            if (FadeToBlack(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.prevMillis = millis();
                curMultiLEDStripEffectData.transitionState = 20;
            }
            break;
        // Wait 200 Milliseconds
        case 20:
            if (millis() - curMultiLEDStripEffectData.prevMillis >= 200)
            {
                curMultiLEDStripEffectData.transitionState = 30;
            }
            break;
        // Fade to blue
        case 30:
            if (FadeToColor(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 40;
            }
            break;
        // Wait 500 Milliseconds
        case 40:
            if (millis() - curMultiLEDStripEffectData.prevMillis >= 500)
            {
                curMultiLEDStripEffectData.transitionState = 0;
            }
            break;
        }
        break;

    case MultiLEDEffect::Idle:
        /*
                Fades all to black
            */
        FadeToBlack(highLevelLEDStripData);
        ResetSingleEffectData();
        prevMultiLEDStripEffectData.multiLEDEffect = curMultiLEDStripEffectData.multiLEDEffect;
        break;

    case MultiLEDEffect::Power:
        /*
                Normal mode which displays the current selected LED effect
            */
        switch (curMultiLEDStripEffectData.transitionState)
        {
        // Check for Mode change
        case 0:
            if (curMultiLEDStripEffectData.multiLEDEffect != prevMultiLEDStripEffectData.multiLEDEffect)
            {
                prevMultiLEDStripEffectData.multiLEDEffect = curMultiLEDStripEffectData.multiLEDEffect;
                curMultiLEDStripEffectData.transitionState = 10;
            }
            else
            {
                // ---- Handle single LED strip effects
                // -- Strip 1
                HandleSingleLEDStripEffects(1, networkLEDStrip1Data);
                // -- Strip 2
                HandleSingleLEDStripEffects(2, networkLEDStrip2Data);
            }
            break;
        // Fade black
        case 10:
            if (FadeToBlack(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 20;
            }
            break;
        // Set new values
        case 20:
            if (SetColor(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 0;
            }
            break;
        }
        break;

    case MultiLEDEffect::MotionDetected:
        /*
                Fades a color in and out 
            */
        ResetSingleEffectData();
        highLevelLEDStripData = getDefaultHigh();

        highLevelLEDStripData.redColorValue = network->stNetworkMotionData.redColorValue;
        highLevelLEDStripData.greenColorValue = network->stNetworkMotionData.greenColorValue;
        highLevelLEDStripData.blueColorValue = network->stNetworkMotionData.blueColorValue;
        highLevelLEDStripData.whiteTemperatureValue = network->stNetworkMotionData.whiteTemperatureValue;
        if (network->stNetworkMotionData.timeBasedBrightnessChangeEnabled)
        {
            highLevelLEDStripData.colorBrightnessValue = getTimeBasedBrightness();
            highLevelLEDStripData.whiteBrightnessValue = getTimeBasedBrightness();
        }
        else
        {
            highLevelLEDStripData.colorBrightnessValue = network->stNetworkMotionData.colorBrightnessValue;
            highLevelLEDStripData.whiteBrightnessValue = network->stNetworkMotionData.whiteBrightnessValue;
        }

        switch (curMultiLEDStripEffectData.transitionState)
        {
        // Check for mode change
        case 0:
            if (curMultiLEDStripEffectData.multiLEDEffect != prevMultiLEDStripEffectData.multiLEDEffect)
            {
                prevMultiLEDStripEffectData.multiLEDEffect = curMultiLEDStripEffectData.multiLEDEffect;
                curMultiLEDStripEffectData.transitionState = 10;
            }
            else
            {
                FadeToColor(highLevelLEDStripData);
            }
            break;
        // Fade black
        case 10:
            if (FadeToBlack(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 20;
            }
            break;
        // Set new values
        case 20:
            highLevelLEDStripData.colorBrightnessValue = 0; // Disable brightness becaue we want to get it faded in
            if (SetColor(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 0;
            }
            break;
        }
        break;

    case MultiLEDEffect::Music:
        /*
                Gets implemented in later version
            */
        curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Idle;
        ResetSingleEffectData();
        break;

    case MultiLEDEffect::Alarm:
        /*
                Gets implemented in later version
            */
        ResetSingleEffectData();
        highLevelLEDStripData = getDefaultHigh();
        lowLevelLEDStripData = getDefaultLow();

        switch (curMultiLEDStripEffectData.transitionState)
        {
        // Check for mode change
        case 0:
            if (curMultiLEDStripEffectData.multiLEDEffect != prevMultiLEDStripEffectData.multiLEDEffect)
            {
                prevMultiLEDStripEffectData.multiLEDEffect = curMultiLEDStripEffectData.multiLEDEffect;
                curMultiLEDStripEffectData.transitionState = 10;
            }
            break;
        // Fade black
        case 10:
            if (FadeToBlack(highLevelLEDStripData))
            {
                curMultiLEDStripEffectData.transitionState = 20;
            }
            break;
        // Alarm Mode
        case 20:
            switch (networkAlarmData.mode)
            {
            /*
                            Does nothing like the name says. Fascinating i know 
                        */
            case AlarmMode::Nothing:
                FadeToBlack(highLevelLEDStripData);
                break;

            /*
                            Pulses 3 times in a slow frequency
                            Color orange
                            Base Brightness 12.5 %
                        */
            case AlarmMode::Warning:

                highLevelLEDStripData.redColorValue = 255;
                highLevelLEDStripData.greenColorValue = 64;
                highLevelLEDStripData.colorBrightnessFadeTime = 1000;

                switch (curMultiLEDStripEffectData.subTransitionState)
                {
                // Set color
                case 0:
                    if (SetColor(highLevelLEDStripData))
                    {
                        alarmFinished = false;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 10:
                    highLevelLEDStripData.colorBrightnessValue = 512;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 20;
                    }
                    break;
                // Pause for 2500 Milliseconds
                case 20:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 2500)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    break;
                // Fade to 100% Brightness
                case 30:
                    highLevelLEDStripData.colorBrightnessValue = 4096;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 40;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 40:
                    highLevelLEDStripData.colorBrightnessValue = 512;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 50;
                    }
                    break;
                // Check Counter or power
                case 50:
                    if (curMultiLEDStripEffectData.counter >= 2)
                    {
                        curMultiLEDStripEffectData.counter = 0;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    else
                    {
                        curMultiLEDStripEffectData.counter++;
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    if (!networkAlarmData.power)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 60;
                    }
                    break;
                // Change color to green
                case 60:
                    highLevelLEDStripData.redColorValue = 0;
                    highLevelLEDStripData.greenColorValue = 255;
                    highLevelLEDStripData.colorBrightnessValue = 4096;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 70;
                    }
                    break;
                // Wait 3000 Milliseconds and set alarmFinished
                case 70:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 3000)
                    {
                        alarmFinished = true;
                    }
                    break;
                }
                break;

            /*
                            Pulses 3 times in a medium frequency
                            Color red
                            Base Brightness 12.5 %
                        */
            case AlarmMode::Error:

                highLevelLEDStripData.redColorValue = 255;
                highLevelLEDStripData.colorBrightnessFadeTime = 500;

                switch (curMultiLEDStripEffectData.subTransitionState)
                {
                // Set color
                case 0:
                    if (SetColor(highLevelLEDStripData))
                    {
                        alarmFinished = false;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 10:
                    highLevelLEDStripData.colorBrightnessValue = 512;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 20;
                    }
                    break;
                // Pause for 1500 Milliseconds
                case 20:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 1500)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    break;
                // Fade to 100% Brightness
                case 30:
                    highLevelLEDStripData.colorBrightnessValue = 4096;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 40;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 40:
                    highLevelLEDStripData.colorBrightnessValue = 512;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 50;
                    }
                    break;
                // Check Counter or power
                case 50:
                    if (curMultiLEDStripEffectData.counter >= 2)
                    {
                        curMultiLEDStripEffectData.counter = 0;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    else
                    {
                        curMultiLEDStripEffectData.counter++;
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    if (!networkAlarmData.power)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 60;
                    }
                    break;
                // Change color to green
                case 60:
                    highLevelLEDStripData.redColorValue = 0;
                    highLevelLEDStripData.greenColorValue = 255;
                    highLevelLEDStripData.colorBrightnessValue = 4096;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 70;
                    }
                    break;
                // Wait 3000 Milliseconds and set alarmFinished
                case 70:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 3000)
                    {
                        alarmFinished = true;
                    }
                    break;
                }
                break;

            /*
                Pulses 3 times in a fast frequency
                Color red, cw
                Base Brightness Red 12.5 %
                Base Brightness Cw 25 %
            */
            case AlarmMode::Critical:
                // ToDo Fix
                /*
                highLevelLEDStripData.redColorValue = 255;
                highLevelLEDStripData.cwColorValue = 255;

                lowLevelLEDStripData.redColorValue = 255;
                lowLevelLEDStripData.redBrightnessFadeTime = 400;
                lowLevelLEDStripData.cwColorValue = 255;
                lowLevelLEDStripData.cwBrightnessValue = 1024;
                lowLevelLEDStripData.cwBrightnessFadeTime = 300;

                switch (curMultiLEDStripEffectData.subTransitionState)
                {
                // Set color
                case 0:
                    if (SetColor(highLevelLEDStripData))
                    {
                        alarmFinished = false;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 10:
                    lowLevelLEDStripData.redBrightnessValue = 512;
                    lowLevelLEDStripData.cwBrightnessValue = 1024;
                    if (FadeToColor(lowLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 20;
                    }
                    break;
                // Pause for 1000 Milliseconds
                case 20:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 1000)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    break;
                // Fade to 100% Brightness
                case 30:
                    lowLevelLEDStripData.redBrightnessValue = 4096;
                    lowLevelLEDStripData.cwBrightnessValue = 256;
                    if (FadeToColor(lowLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 40;
                    }
                    break;
                // Fade to 12.5% Brightness
                case 40:
                    lowLevelLEDStripData.redBrightnessValue = 512;
                    lowLevelLEDStripData.cwBrightnessValue = 256;
                    if (FadeToColor(lowLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.subTransitionState = 50;
                    }
                    break;
                // Check Counter or power
                case 50:
                    if (curMultiLEDStripEffectData.counter >= 2)
                    {
                        curMultiLEDStripEffectData.counter = 0;
                        curMultiLEDStripEffectData.subTransitionState = 10;
                    }
                    else
                    {
                        curMultiLEDStripEffectData.counter++;
                        curMultiLEDStripEffectData.subTransitionState = 30;
                    }
                    if (!networkAlarmData.power)
                    {
                        curMultiLEDStripEffectData.subTransitionState = 60;
                    }
                    break;
                // Change color to green
                case 60:
                    highLevelLEDStripData.redColorValue = 0;
                    highLevelLEDStripData.greenColorValue = 255;
                    highLevelLEDStripData.cwColorValue = 0;
                    highLevelLEDStripData.brightnessValue = 4096;
                    if (FadeToColor(highLevelLEDStripData))
                    {
                        curMultiLEDStripEffectData.prevMillis = millis();
                        curMultiLEDStripEffectData.subTransitionState = 70;
                    }
                    break;
                // Wait 3000 Milliseconds and set alarmFinished
                case 70:
                    if (millis() - curMultiLEDStripEffectData.prevMillis >= 3000)
                    {
                        alarmFinished = true;
                    }
                    break;
                }
                */
                break;
            }
            break;
        }
        break;

    default:
        /*
                Fades all to black
            */
        curMultiLEDStripEffectData.multiLEDEffect = MultiLEDEffect::Idle;
        ResetSingleEffectData();
        break;
    }
};

/**
 * Handels the display of single LED strip effects
 * 
 * @parameter stripID                      The ID of the used led strip 
 * @parameter commandNetworkLEDStripData   NetworkLEDStripData for the used LED strip define in stripID
 */
void LedDriver::HandleSingleLEDStripEffects(uint8_t stripID,
                                            NetworkLEDStripData commandNetworkLEDStripData)
{

    // Get Effect data
    SingleLEDStripEffectData *effectData = getSingleLEDStripEffectData(stripID);
    HighLevelLEDStripData highLevelLEDStripData = getDefaultHigh();

    // Check fo power
    if (commandNetworkLEDStripData.power)
    {

        // Check for effect change
        if (commandNetworkLEDStripData.effect != effectData->singleLEDEffect)
        {
            effectData->transitionState = 0;
            effectData->subTransitionState = 0;
            effectData->singleLEDEffect = commandNetworkLEDStripData.effect;
        }

        // effect change state machine
        switch (effectData->transitionState)
        {

            // Fade Strip to black
        case 0:
            if (FadeToBlack(stripID, highLevelLEDStripData))
            {
                SetColor(stripID, commandNetworkLEDStripData)
                effectData->transitionState = 10;
            }
            break;

            // Display effect
        case 10:

            switch (commandNetworkLEDStripData.effect)
            {

            case SingleLEDEffect::None:
                FadeToColor(stripID, commandNetworkLEDStripData);
                break;

            case SingleLEDEffect::TriplePulse:

                break;

            case SingleLEDEffect::Rainbow:
                highLevelLEDStripData.colorBrightnessValue = commandNetworkLEDStripData.ledStripData.colorBrightnessValue;
                highLevelLEDStripData.colorFadeTime = 6000;
                switch (effectData->subTransitionState)
                {
                    // Fade to red
                case 0:
                    highLevelLEDStripData.redColorValue = 255;
                    if (FadeToColor(stripID, highLevelLEDStripData))
                    {
                        effectData->subTransitionState = 10;
                    }
                    break;
                    // Fade to green
                case 10:
                    highLevelLEDStripData.greenColorValue = 255;
                    if (FadeToColor(stripID, highLevelLEDStripData))
                    {
                        effectData->subTransitionState = 20;
                    }
                    break;
                    // Fade to blue
                case 20:
                    highLevelLEDStripData.blueColorValue = 255;
                    if (FadeToColor(stripID, highLevelLEDStripData))
                    {
                        effectData->subTransitionState = 0;
                    }
                    break;
                }
            }

            break;
        }
    }
    else
    {
        FadeToBlack(stripID, highLevelLEDStripData);
    }

    // ToDo fix
    /*
            // Get used network data
            highLevelLEDStripData.redColorValue = commandNetworkLEDStripData.ledStripData.redColorValue;
            highLevelLEDStripData.greenColorValue = commandNetworkLEDStripData.ledStripData.greenColorValue;
            highLevelLEDStripData.blueColorValue = commandNetworkLEDStripData.ledStripData.blueColorValue;
            highLevelLEDStripData.cwColorValue = commandNetworkLEDStripData.ledStripData.cwColorValue;
            highLevelLEDStripData.wwColorValue = commandNetworkLEDStripData.ledStripData.wwColorValue;
            // Effect procedure
            switch (effectData->transitionState)
            {
            // Check for Effect change
            case 0:
                if (commandNetworkLEDStripData.effect != effectData->singleLEDEffect)
                {
                    effectData->singleLEDEffect = commandNetworkLEDStripData.effect;
                    effectData->transitionState = 10;
                }
                else
                {
                    effectData->transitionState = 20;
                }
                break;
            // Fade black
            case 10:
                if (FadeToBlack(stripID, highLevelLEDStripData))
                {
                    effectData->transitionState = 20;
                }
                break;
            // Fade to 12,5% brightness
            case 20:
                highLevelLEDStripData.brightnessValue = 512;
                highLevelLEDStripData.brightnessFadeTime = 800;
                if (FadeToColor(stripID, highLevelLEDStripData))
                {
                    effectData->prevMillis = millis();
                    effectData->transitionState = 25;
                }
                break;
            // Wait 2000 Milliseconds
            case 25:
                if (millis() - effectData->prevMillis >= 2000)
                {
                    effectData->transitionState = 30;
                }
                break;
            // Fade to 100% brightness
            case 30:
                highLevelLEDStripData.brightnessValue = 4095;
                highLevelLEDStripData.brightnessFadeTime = 400;
                if (FadeToColor(stripID, highLevelLEDStripData))
                {
                    effectData->prevMillis = millis();
                    effectData->transitionState = 35;
                }
                break;
            // Wait 200 Milliseconds
            case 35:
                if (millis() - effectData->prevMillis >= 100)
                {
                    effectData->transitionState = 40;
                }
                break;
            // Fade to 12,5%
            case 40:
                highLevelLEDStripData.brightnessValue = 512;
                highLevelLEDStripData.brightnessFadeTime = 400;
                if (FadeToColor(stripID, highLevelLEDStripData))
                {
                    effectData->transitionState = 45;
                }
                break;
            // Check counter
            case 45:
                if (effectData->counter >= 2)
                {
                    effectData->counter = 0;
                    effectData->prevMillis = millis();
                    effectData->transitionState = 25;
                }
                else
                {
                    effectData->counter++;
                    effectData->prevMillis = millis();
                    effectData->transitionState = 50;
                }
                break;
            // Wait 400 Milliseconds
            case 50:
                if (millis() - effectData->prevMillis >= 400)
                {
                    effectData->transitionState = 30;
                }
                break;
            }
            */
};

/**
 * Returns a brightness value based on the current time
 * 
 * @return Time baed brightness value
 */
uint16_t LedDriver::getTimeBasedBrightness()
{
    uint16_t brightness = 4096;

    switch (network->stNetworkTimeData.hour)
    {
    // 100%
    case 20:
        brightness = 4095;
        break;
    // 75%
    case 21:
        brightness = 3096;
        break;
    // 50%
    case 22:
        brightness = 2048;
        break;
    // 37,5%
    case 23:
        brightness = 1536;
        break;
    // 25%
    case 24:
        brightness = 1024;
        break;
    // 12,5%
    case 0:
        brightness = 512;
        break;
    // 12,5%
    case 1:
        brightness = 512;
        break;
    // 12,5%
    case 2:
        brightness = 512;
        break;
    // 12,5%
    case 3:
        brightness = 512;
        break;
    // 25%
    case 4:
        brightness = 1024;
        break;
    // 37,5%
    case 5:
        brightness = 1536;
        break;
    // 50%
    case 6:
        brightness = 2048;
        break;
    // 75%
    case 7:
        brightness = 3096;
        break;
    // 100%
    case 8:
        brightness = 4095;
        break;
    }

    return brightness;
};

void LedDriver::ResetSingleEffectData()
{
    // Strip 1
    SingleLEDStripEffectData *effectDataStrip1 = getSingleLEDStripEffectData(1);
    effectDataStrip1->transitionState = 0;
    effectDataStrip1->prevMillis = 0;
    effectDataStrip1->counter = 0;
    // Strip 2
    SingleLEDStripEffectData *effectDataStrip2 = getSingleLEDStripEffectData(2);
    effectDataStrip2->transitionState = 0;
    effectDataStrip2->prevMillis = 0;
    effectDataStrip2->counter = 0;
};

/**
 * Returns a pointer to the current LEDEffectData of the coresponding stripID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return Pointer to the current LEDEffectData of the given stripID
 */
SingleLEDStripEffectData *LedDriver::getSingleLEDStripEffectData(uint8_t stripID)
{

    // LED Strip 1
    if (stripID == 1)
    {
        return &singleLEDStrip1EffectData;
    }

    // LED Strip 2
    if (stripID == 2)
    {
        return &singleLEDStrip2EffectData;
    }
};

// # ================================================================ ================================================================ # //
// #                                                             SET COLOR                                                             # //
// # ================================================================ ================================================================ # //

bool LedDriver::SetColor(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeFinished = FadeToColor(stripID, commandNetworkLEDStripData.ledStripData);
    return fadeFinished;
};

bool LedDriver::SetColor(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData)
{
    // Increase fade speed to instant
    commandHighLevelLEDStripData.colorFadeTime = 0;
    commandHighLevelLEDStripData.colorFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.colorBrightnessFadeTime = 0;
    commandHighLevelLEDStripData.colorBrightnessFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.whiteTemperatureFadeTime = 0;
    commandHighLevelLEDStripData.whiteTemperatureFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.whiteBrightnessFadeTime = 0;
    commandHighLevelLEDStripData.whiteBrightnessFadeCurve = FadeCurve::None;

    bool fadeFinished = FadeToColor(stripID,
                                    commandHighLevelLEDStripData);
    return fadeFinished;
};

bool LedDriver::SetColor(NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeFinished = SetColor(commandNetworkLEDStripData.ledStripData);
    return fadeFinished;
};

bool LedDriver::SetColor(HighLevelLEDStripData commandHighLevelLEDStripData)
{
    // Increase fade speed to instant
    commandHighLevelLEDStripData.colorFadeTime = 0;
    commandHighLevelLEDStripData.colorFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.colorBrightnessFadeTime = 0;
    commandHighLevelLEDStripData.colorBrightnessFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.whiteTemperatureFadeTime = 0;
    commandHighLevelLEDStripData.whiteTemperatureFadeCurve = FadeCurve::None;
    commandHighLevelLEDStripData.whiteBrightnessFadeTime = 0;
    commandHighLevelLEDStripData.whiteBrightnessFadeCurve = FadeCurve::None;

    bool fadeStrip1Finished = FadeToColor(1, commandHighLevelLEDStripData);
    bool fadeStrip2Finished = FadeToColor(2, commandHighLevelLEDStripData);
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

// # ================================================================ ================================================================ # //
// #                                                            FADE COLOR                                                             # //
// # ================================================================ ================================================================ # //

bool LedDriver::FadeToColor(uint8_t stripID,
                            NetworkLEDStripData commandNetworkLEDStripData)
{
    // Add missing default values for NetworkLEDStripData
    commandNetworkLEDStripData.ledStripData.colorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
    commandNetworkLEDStripData.ledStripData.colorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
    commandNetworkLEDStripData.ledStripData.colorBrightnessFadeTime = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
    commandNetworkLEDStripData.ledStripData.colorBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;
    commandNetworkLEDStripData.ledStripData.whiteTemperatureFadeTime = defaultHighLevelLEDStripData.whiteTemperatureFadeTime;
    commandNetworkLEDStripData.ledStripData.whiteTemperatureFadeCurve = defaultHighLevelLEDStripData.whiteTemperatureFadeCurve;
    commandNetworkLEDStripData.ledStripData.whiteBrightnessFadeTime = defaultHighLevelLEDStripData.whiteBrightnessFadeTime;
    commandNetworkLEDStripData.ledStripData.whiteBrightnessFadeCurve = defaultHighLevelLEDStripData.whiteBrightnessFadeCurve;

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
    lowLevelLEDStripData.redColorValue = commandHighLevelLEDStripData.redColorValue;
    lowLevelLEDStripData.redColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.redColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.redBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.redBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.redBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- GREEN
    // -- Color
    lowLevelLEDStripData.greenColorValue = commandHighLevelLEDStripData.greenColorValue;
    lowLevelLEDStripData.greenColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.greenColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.greenBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.greenBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.greenBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- BLUE
    // -- Color
    lowLevelLEDStripData.blueColorValue = commandHighLevelLEDStripData.blueColorValue;
    lowLevelLEDStripData.blueColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.blueColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.blueBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.blueBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.blueBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- CW
    // -- Color
    if (commandHighLevelLEDStripData.whiteTemperatureValue >= 250)
    {

        lowLevelLEDStripData.cwColorValue = map(commandHighLevelLEDStripData.whiteTemperatureValue, 500, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.cwColorValue = 255;
    }
    lowLevelLEDStripData.cwColorFadeTime = commandHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.cwColorFadeCurve = commandHighLevelLEDStripData.whiteTemperatureFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.cwBrightnessValue = commandHighLevelLEDStripData.whiteBrightnessValue;
    lowLevelLEDStripData.cwBrightnessFadeTime = commandHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.cwBrightnessFadeCurve = commandHighLevelLEDStripData.whiteBrightnessFadeCurve;

    // ---- WW
    // -- Color
    if (commandHighLevelLEDStripData.whiteTemperatureValue <= 250)
    {
        lowLevelLEDStripData.wwColorValue = map(commandHighLevelLEDStripData.whiteTemperatureValue, 0, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.wwColorValue = 255;
    }
    lowLevelLEDStripData.wwColorFadeTime = commandHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.wwColorFadeCurve = commandHighLevelLEDStripData.whiteTemperatureFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.wwBrightnessValue = commandHighLevelLEDStripData.whiteBrightnessValue;
    lowLevelLEDStripData.wwBrightnessFadeTime = commandHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.wwBrightnessFadeCurve = commandHighLevelLEDStripData.whiteBrightnessFadeCurve;

    // Call low level fade
    bool fadeFinished = FadeToColor(stripID,
                                    lowLevelLEDStripData);
    return fadeFinished;
};

bool LedDriver::FadeToColor(uint8_t stripID,
                            LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeFinished = true;
    unsigned long curMillis = previousMillisRefreshRate;

    // Get current data of strip
    LEDStripData *ptrCurrentLEDStripData = getCurrentLEDStripData(stripID);
    LEDStripData *ptrPreviousLEDStripData = getPreviousLEDStripData(stripID);

    // Check if FadeToColor got called last cycle
    if ((ptrCurrentLEDStripData->lastRefreshRateCount + 1) != refreshRateCounter)
    {
        // ---- Update fade times
        // -- RED
        ptrCurrentLEDStripData->prevMillisRedColorFade = curMillis;
        ptrCurrentLEDStripData->prevMillisRedBrightnessFade = curMillis;
        // -- GREEN
        ptrCurrentLEDStripData->prevMillisGreenColorFade = curMillis;
        ptrCurrentLEDStripData->prevMillisGreenBrightnessFade = curMillis;
        // -- BLUE
        ptrCurrentLEDStripData->prevMillisBlueColorFade = curMillis;
        ptrCurrentLEDStripData->prevMillisBlueBrightnessFade = curMillis;
        // -- CW
        ptrCurrentLEDStripData->prevMillisCwColorFade = curMillis;
        ptrCurrentLEDStripData->prevMillisCwBrightnessFade = curMillis;
        // -- WW
        ptrCurrentLEDStripData->prevMillisWwColorFade = curMillis;
        ptrCurrentLEDStripData->prevMillisWwBrightnessFade = curMillis;

        ptrCurrentLEDStripData->lastRefreshRateCount = refreshRateCounter;
    }
    else
    {
        ptrCurrentLEDStripData->lastRefreshRateCount = refreshRateCounter;
    }

    // ---- RED
    // -- Color
    if (ptrCurrentLEDStripData->redColorValue != commandLowLevelLEDStripData.redColorValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.redColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisRedColorFade) / (double)commandLowLevelLEDStripData.redColorFadeTime;
        }
        ptrCurrentLEDStripData->redColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.redColorFadeCurve,
                                                                       percent,
                                                                       ptrCurrentLEDStripData->prevRedColorValue,
                                                                       commandLowLevelLEDStripData.redColorValue);
        if (ptrCurrentLEDStripData->redColorValue == commandLowLevelLEDStripData.redColorValue)
        {
            ptrCurrentLEDStripData->prevRedColorValue = ptrCurrentLEDStripData->redColorValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisRedColorFade = curMillis;
    }
    // -- Brightness
    if (ptrCurrentLEDStripData->redBrightnessValue != commandLowLevelLEDStripData.redBrightnessValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.redBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisRedBrightnessFade) / (double)commandLowLevelLEDStripData.redBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->redBrightnessValue = getCurveValue(commandLowLevelLEDStripData.redBrightnessFadeCurve,
                                                                   percent,
                                                                   ptrCurrentLEDStripData->prevRedBrightnessValue,
                                                                   commandLowLevelLEDStripData.redBrightnessValue);

        if (ptrCurrentLEDStripData->redBrightnessValue == commandLowLevelLEDStripData.redBrightnessValue)
        {
            ptrCurrentLEDStripData->prevRedBrightnessValue = ptrCurrentLEDStripData->redBrightnessValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisRedBrightnessFade = curMillis;
    }

    // ---- GREEN
    // -- Color
    if (ptrCurrentLEDStripData->greenColorValue != commandLowLevelLEDStripData.greenColorValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.greenColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisGreenColorFade) / (double)commandLowLevelLEDStripData.greenColorFadeTime;
        }
        ptrCurrentLEDStripData->greenColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.greenColorFadeCurve,
                                                                         percent,
                                                                         ptrCurrentLEDStripData->prevGreenColorValue,
                                                                         commandLowLevelLEDStripData.greenColorValue);
        if (ptrCurrentLEDStripData->greenColorValue == commandLowLevelLEDStripData.greenColorValue)
        {
            ptrCurrentLEDStripData->prevGreenColorValue = ptrCurrentLEDStripData->greenColorValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisGreenColorFade = curMillis;
    }
    // -- Brightness
    if (ptrCurrentLEDStripData->greenBrightnessValue != commandLowLevelLEDStripData.greenBrightnessValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.greenBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisGreenBrightnessFade) / (double)commandLowLevelLEDStripData.greenBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->greenBrightnessValue = getCurveValue(commandLowLevelLEDStripData.greenBrightnessFadeCurve,
                                                                     percent,
                                                                     ptrCurrentLEDStripData->prevGreenBrightnessValue,
                                                                     commandLowLevelLEDStripData.greenBrightnessValue);

        if (ptrCurrentLEDStripData->greenBrightnessValue == commandLowLevelLEDStripData.greenBrightnessValue)
        {
            ptrCurrentLEDStripData->prevGreenBrightnessValue = ptrCurrentLEDStripData->redBrightnessValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisGreenBrightnessFade = curMillis;
    }

    // ---- BLUE
    // -- Color
    if (ptrCurrentLEDStripData->blueColorValue != commandLowLevelLEDStripData.blueColorValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.blueColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisBlueColorFade) / (double)commandLowLevelLEDStripData.blueColorFadeTime;
        }
        ptrCurrentLEDStripData->blueColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.blueColorFadeCurve,
                                                                        percent,
                                                                        ptrCurrentLEDStripData->prevBlueColorValue,
                                                                        commandLowLevelLEDStripData.blueColorValue);
        if (ptrCurrentLEDStripData->blueColorValue == commandLowLevelLEDStripData.blueColorValue)
        {
            ptrCurrentLEDStripData->prevBlueColorValue = ptrCurrentLEDStripData->blueColorValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisBlueColorFade = curMillis;
    }
    // -- Brightness
    if (ptrCurrentLEDStripData->blueBrightnessValue != commandLowLevelLEDStripData.blueBrightnessValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.blueBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisBlueBrightnessFade) / (double)commandLowLevelLEDStripData.blueBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->blueBrightnessValue = getCurveValue(commandLowLevelLEDStripData.blueBrightnessFadeCurve,
                                                                    percent,
                                                                    ptrCurrentLEDStripData->prevBlueBrightnessValue,
                                                                    commandLowLevelLEDStripData.blueBrightnessValue);

        if (ptrCurrentLEDStripData->blueBrightnessValue == commandLowLevelLEDStripData.blueBrightnessValue)
        {
            ptrCurrentLEDStripData->prevBlueBrightnessValue = ptrCurrentLEDStripData->blueBrightnessValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisBlueBrightnessFade = curMillis;
    }

    // ---- CW
    // -- Color
    if (ptrCurrentLEDStripData->cwColorValue != commandLowLevelLEDStripData.cwColorValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.cwColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisCwColorFade) / (double)commandLowLevelLEDStripData.cwColorFadeTime;
        }
        ptrCurrentLEDStripData->cwColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.cwColorFadeCurve,
                                                                      percent,
                                                                      ptrCurrentLEDStripData->prevCwColorValue,
                                                                      commandLowLevelLEDStripData.cwColorValue);
        if (ptrCurrentLEDStripData->cwColorValue == commandLowLevelLEDStripData.cwColorValue)
        {
            ptrCurrentLEDStripData->prevCwColorValue = ptrCurrentLEDStripData->cwColorValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisCwColorFade = curMillis;
    }
    // -- Brightness
    if (ptrCurrentLEDStripData->cwBrightnessValue != commandLowLevelLEDStripData.cwBrightnessValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.cwBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisCwBrightnessFade) / (double)commandLowLevelLEDStripData.cwBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->cwBrightnessValue = getCurveValue(commandLowLevelLEDStripData.cwBrightnessFadeCurve,
                                                                  percent,
                                                                  ptrCurrentLEDStripData->prevCwBrightnessValue,
                                                                  commandLowLevelLEDStripData.cwBrightnessValue);

        if (ptrCurrentLEDStripData->cwBrightnessValue == commandLowLevelLEDStripData.cwBrightnessValue)
        {
            ptrCurrentLEDStripData->prevCwBrightnessValue = ptrCurrentLEDStripData->cwBrightnessValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisCwBrightnessFade = curMillis;
    }

    // ---- WW
    // -- Color
    if (ptrCurrentLEDStripData->wwColorValue != commandLowLevelLEDStripData.wwColorValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.wwColorFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisWwColorFade) / (double)commandLowLevelLEDStripData.wwColorFadeTime;
        }
        ptrCurrentLEDStripData->wwColorValue = (uint8_t)getCurveValue(commandLowLevelLEDStripData.wwColorFadeCurve,
                                                                      percent,
                                                                      ptrCurrentLEDStripData->prevWwColorValue,
                                                                      commandLowLevelLEDStripData.wwColorValue);
        if (ptrCurrentLEDStripData->wwColorValue == commandLowLevelLEDStripData.wwColorValue)
        {
            ptrCurrentLEDStripData->prevWwColorValue = ptrCurrentLEDStripData->wwColorValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisWwColorFade = curMillis;
    }
    // -- Brightness
    if (ptrCurrentLEDStripData->wwBrightnessValue != commandLowLevelLEDStripData.wwBrightnessValue)
    {
        double percent = 0.0;
        if (commandLowLevelLEDStripData.wwBrightnessFadeTime != 0)
        {
            percent = (curMillis - ptrCurrentLEDStripData->prevMillisWwBrightnessFade) / (double)commandLowLevelLEDStripData.wwBrightnessFadeTime;
        }
        ptrCurrentLEDStripData->wwBrightnessValue = getCurveValue(commandLowLevelLEDStripData.wwBrightnessFadeCurve,
                                                                  percent,
                                                                  ptrCurrentLEDStripData->prevWwBrightnessValue,
                                                                  commandLowLevelLEDStripData.wwBrightnessValue);

        if (ptrCurrentLEDStripData->wwBrightnessValue == commandLowLevelLEDStripData.wwBrightnessValue)
        {
            ptrCurrentLEDStripData->prevWwBrightnessValue = ptrCurrentLEDStripData->wwBrightnessValue;
        }
        fadeFinished = false;
    }
    else
    {
        ptrCurrentLEDStripData->prevMillisWwBrightnessFade = curMillis;
    }

    ptrCurrentLEDStripData->fadeFinished = fadeFinished;
    return fadeFinished;
};

bool LedDriver::FadeToColor(NetworkLEDStripData commandHighLevelLEDStripData)
{
    bool fadeFinished = FadeToColor(commandHighLevelLEDStripData.ledStripData);
    return fadeFinished;
};

bool LedDriver::FadeToColor(HighLevelLEDStripData commandHighLevelLEDStripData)
{
    // Create Low Level data
    LowLevelLEDStripData lowLevelLEDStripData;
    // ---- RED
    // -- Color
    lowLevelLEDStripData.redColorValue = commandHighLevelLEDStripData.redColorValue;
    lowLevelLEDStripData.redColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.redColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.redBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.redBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.redBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- GREEN
    // -- Color
    lowLevelLEDStripData.greenColorValue = commandHighLevelLEDStripData.greenColorValue;
    lowLevelLEDStripData.greenColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.greenColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.greenBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.greenBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.greenBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- BLUE
    // -- Color
    lowLevelLEDStripData.blueColorValue = commandHighLevelLEDStripData.blueColorValue;
    lowLevelLEDStripData.blueColorFadeTime = commandHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.blueColorFadeCurve = commandHighLevelLEDStripData.colorFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.blueBrightnessValue = commandHighLevelLEDStripData.colorBrightnessValue;
    lowLevelLEDStripData.blueBrightnessFadeTime = commandHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.blueBrightnessFadeCurve = commandHighLevelLEDStripData.colorBrightnessFadeCurve;

    // ---- CW
    // -- Color
    if (commandHighLevelLEDStripData.whiteTemperatureValue >= 250)
    {

        lowLevelLEDStripData.cwColorValue = map(commandHighLevelLEDStripData.whiteTemperatureValue, 500, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.cwColorValue = 255;
    }
    lowLevelLEDStripData.cwColorFadeTime = commandHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.cwColorFadeCurve = commandHighLevelLEDStripData.whiteTemperatureFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.cwBrightnessValue = commandHighLevelLEDStripData.whiteBrightnessValue;
    lowLevelLEDStripData.cwBrightnessFadeTime = commandHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.cwBrightnessFadeCurve = commandHighLevelLEDStripData.whiteBrightnessFadeCurve;

    // ---- WW
    // -- Color
    if (commandHighLevelLEDStripData.whiteTemperatureValue <= 250)
    {
        lowLevelLEDStripData.wwColorValue = map(commandHighLevelLEDStripData.whiteTemperatureValue, 0, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.wwColorValue = 255;
    }
    lowLevelLEDStripData.wwColorFadeTime = commandHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.wwColorFadeCurve = commandHighLevelLEDStripData.whiteTemperatureFadeCurve;
    // -- Brightness
    lowLevelLEDStripData.wwBrightnessValue = commandHighLevelLEDStripData.whiteBrightnessValue;
    lowLevelLEDStripData.wwBrightnessFadeTime = commandHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.wwBrightnessFadeCurve = commandHighLevelLEDStripData.whiteBrightnessFadeCurve;

    bool fadeFinished = FadeToColor(lowLevelLEDStripData);
    return fadeFinished;
};

bool LedDriver::FadeToColor(LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, commandLowLevelLEDStripData);
    bool fadeStrip2Finished = FadeToColor(2, commandLowLevelLEDStripData);
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

// # ================================================================ ================================================================ # //
// #                                                           FADE TO BLACK                                                           # //
// # ================================================================ ================================================================ # //

bool LedDriver::FadeToBlack(uint8_t stripID,
                            NetworkLEDStripData commandNetworkLEDStripData)
{
    // Call high level fade to black
    bool fadeFinished = FadeToBlack(stripID, getDefaultHigh());
    return fadeFinished;
};

bool LedDriver::FadeToBlack(uint8_t stripID,
                            HighLevelLEDStripData commandHighLevelLEDStripData)
{
    // Call low level fade to black
    bool fadeFinished = FadeToBlack(stripID, getDefaultLow());
    return fadeFinished;
};

bool LedDriver::FadeToBlack(uint8_t stripID,
                            LowLevelLEDStripData commandLowLevelLEDStripData)
{
    // We need to get lowe level default or else the white value wont work with high level because of the temperature value
    // Call low level fade to color
    bool fadeFinished = FadeToColor(stripID, getDefaultHigh());
    return fadeFinished;
};

bool LedDriver::FadeToBlack(NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeFinished = FadeToBlack(commandNetworkLEDStripData.ledStripData);
    return fadeFinished;
};

bool LedDriver::FadeToBlack(HighLevelLEDStripData commandHighLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToBlack(1, commandHighLevelLEDStripData);
    bool fadeStrip2Finished = FadeToBlack(2, commandHighLevelLEDStripData);
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

// # ================================================================ ================================================================ # //
// #                                                             LED STRIP                                                             # //
// # ================================================================ ================================================================ # //

/**
 * Updates all led channels to the current LED strip data if the changed from the previous value
 * 
 * @parameter stripID  The ID of the used led strip
 **/
void LedDriver::UpdateLEDStrip(uint8_t stripID)
{
    // Get current data of strip
    LEDStripData *ptrCurrentLEDStripData = getCurrentLEDStripData(stripID);
    LEDStripData *ptrPreviousLEDStripData = getPreviousLEDStripData(stripID);

    // Get led reg of strip
    LEDStripColorReg STRIP = getColorRegForLEDStrip(stripID);

    // Phase shift
    uint16_t phaseShiftRed = 0;
    uint16_t phaseShiftCw = 819;
    uint16_t phaseShiftGreen = 1639;
    uint16_t phaseShiftWw = 2459;
    uint16_t phaseShiftBlue = 3279;

    // ---- Update each color channel
    // -- RED
    if (ptrCurrentLEDStripData->redColorValue != ptrPreviousLEDStripData->redColorValue || ptrCurrentLEDStripData->redBrightnessValue != ptrPreviousLEDStripData->redBrightnessValue)
    {
        UpdateLEDChannel(STRIP.RED_REG,
                         phaseShiftRed,
                         ptrCurrentLEDStripData->redColorValue,
                         ptrCurrentLEDStripData->redBrightnessValue);

        ptrPreviousLEDStripData->redColorValue = ptrCurrentLEDStripData->redColorValue;
        ptrPreviousLEDStripData->redBrightnessValue = ptrCurrentLEDStripData->redBrightnessValue;
    }

    // -- GREEN
    if (ptrCurrentLEDStripData->greenColorValue != ptrPreviousLEDStripData->greenColorValue || ptrCurrentLEDStripData->greenBrightnessValue != ptrPreviousLEDStripData->greenBrightnessValue)
    {
        UpdateLEDChannel(STRIP.GREEN_REG,
                         phaseShiftGreen,
                         ptrCurrentLEDStripData->greenColorValue,
                         ptrCurrentLEDStripData->greenBrightnessValue);

        ptrPreviousLEDStripData->greenColorValue = ptrCurrentLEDStripData->greenColorValue;
        ptrPreviousLEDStripData->greenBrightnessValue = ptrCurrentLEDStripData->greenBrightnessValue;
    }

    // -- BLUE
    if (ptrCurrentLEDStripData->blueColorValue != ptrPreviousLEDStripData->blueColorValue || ptrCurrentLEDStripData->blueBrightnessValue != ptrPreviousLEDStripData->blueBrightnessValue)
    {
        UpdateLEDChannel(STRIP.BLUE_REG,
                         phaseShiftBlue,
                         ptrCurrentLEDStripData->blueColorValue,
                         ptrCurrentLEDStripData->blueBrightnessValue);

        ptrPreviousLEDStripData->blueColorValue = ptrCurrentLEDStripData->blueColorValue;
        ptrPreviousLEDStripData->blueBrightnessValue = ptrCurrentLEDStripData->blueBrightnessValue;
    }

    // -- CW
    if (ptrCurrentLEDStripData->cwColorValue != ptrPreviousLEDStripData->cwColorValue || ptrCurrentLEDStripData->cwBrightnessValue != ptrPreviousLEDStripData->cwBrightnessValue)
    {
        UpdateLEDChannel(STRIP.CW_REG,
                         phaseShiftCw,
                         ptrCurrentLEDStripData->cwColorValue,
                         ptrCurrentLEDStripData->cwBrightnessValue);

        ptrPreviousLEDStripData->cwColorValue = ptrCurrentLEDStripData->cwColorValue;
        ptrPreviousLEDStripData->cwBrightnessValue = ptrCurrentLEDStripData->cwBrightnessValue;
    }

    // -- WW
    if (ptrCurrentLEDStripData->wwColorValue != ptrPreviousLEDStripData->wwColorValue || ptrCurrentLEDStripData->wwBrightnessValue != ptrPreviousLEDStripData->wwBrightnessValue)
    {
        UpdateLEDChannel(STRIP.WW_REG,
                         phaseShiftWw,
                         ptrCurrentLEDStripData->wwColorValue,
                         ptrCurrentLEDStripData->wwBrightnessValue);

        ptrPreviousLEDStripData->wwColorValue = ptrCurrentLEDStripData->wwColorValue;
        ptrPreviousLEDStripData->wwBrightnessValue = ptrCurrentLEDStripData->wwBrightnessValue;
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
    if (brightnessValue == 0 || colorValue == 0)
    {
        data = 0;
    }
    else
    {
        data = (uint16_t)(((double)(colorValue + 1) * 16.0) * ((double)brightnessValue / 4095.0));
    }

    // Bound Check
    if (data >= 4096)
    {
        data = 4095;
    }

    // LED_ON_REG
    uint16_t ON_REG = phaseShift;
    i2c->write8(i2cAddress, REG.ON_L, lowByte(ON_REG));
    i2c->write8(i2cAddress, REG.ON_H, highByte(ON_REG));

    // LED_OFF_REG
    uint16_t OFF_REG = 0;
    if (data + phaseShift <= 4095.0)
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
    if (percent >= 1.0)
    {
        percent = 1.0;
    }
    if (percent <= 0.0)
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
 * Returns a HighLevelLEDStripData struct with default values
 * 
 * @return The HighLevelLEDStripData with default values
 */
HighLevelLEDStripData LedDriver::getDefaultHigh()
{
    return defaultHighLevelLEDStripData;
};

/**
 * Returns a HighLevelLEDStripData struct with default values
 * 
 * @return The HighLevelLEDStripData with default values
 */
LowLevelLEDStripData LedDriver::getDefaultLow()
{
    LowLevelLEDStripData lowLevelLEDStripData;

    // ---- Set default values
    // -- RED
    lowLevelLEDStripData.redColorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.redColorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
    lowLevelLEDStripData.redBrightnessValue = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.redBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;
    // -- GREEN
    lowLevelLEDStripData.greenColorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.greenColorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
    lowLevelLEDStripData.greenBrightnessFadeTime = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.greenBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;
    // -- BLUE
    lowLevelLEDStripData.blueColorFadeTime = defaultHighLevelLEDStripData.colorFadeTime;
    lowLevelLEDStripData.blueColorFadeCurve = defaultHighLevelLEDStripData.colorFadeCurve;
    lowLevelLEDStripData.blueBrightnessFadeTime = defaultHighLevelLEDStripData.colorBrightnessFadeTime;
    lowLevelLEDStripData.blueBrightnessFadeCurve = defaultHighLevelLEDStripData.colorBrightnessFadeCurve;

    // -- CW
    lowLevelLEDStripData.cwColorFadeTime = defaultHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.cwColorFadeCurve = defaultHighLevelLEDStripData.whiteTemperatureFadeCurve;
    lowLevelLEDStripData.cwBrightnessFadeTime = defaultHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.cwBrightnessFadeCurve = defaultHighLevelLEDStripData.whiteBrightnessFadeCurve;
    // -- WW
    lowLevelLEDStripData.wwColorFadeTime = defaultHighLevelLEDStripData.whiteTemperatureFadeTime;
    lowLevelLEDStripData.wwColorFadeCurve = defaultHighLevelLEDStripData.whiteTemperatureFadeCurve;
    lowLevelLEDStripData.wwBrightnessFadeTime = defaultHighLevelLEDStripData.whiteBrightnessFadeTime;
    lowLevelLEDStripData.wwBrightnessFadeCurve = defaultHighLevelLEDStripData.whiteBrightnessFadeCurve;

    return lowLevelLEDStripData;
};

/**
 * Returns a pointer to the current LEDStripData of the coresponding stripID
 * 
 * @parameter stripID   Strip ID of the LED strip
 * 
 * @return Pointer to the current LEDStripData of the given stripID
 */
LEDStripData *LedDriver::getCurrentLEDStripData(uint8_t stripID)
{

    // LED Strip 1
    if (stripID == 1)
    {
        return &currentLEDStrip1Data;
    }

    // LED Strip 2
    if (stripID == 2)
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
LEDStripData *LedDriver::getPreviousLEDStripData(uint8_t stripID)
{

    // LED Strip 1
    if (stripID == 1)
    {
        return &prevLEDStrip1Data;
    }

    // LED Strip 2
    if (stripID == 2)
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
    if (stripID == 1)
    {
        LEDStripColorReg STRIP;

        // CW
        STRIP.CW_REG.ON_L = LED3_ON_L;   //= LED6_ON_L;
        STRIP.CW_REG.ON_H = LED3_ON_H;   //= LED6_ON_H;
        STRIP.CW_REG.OFF_L = LED3_OFF_L; //= LED6_OFF_L;
        STRIP.CW_REG.OFF_H = LED3_OFF_H; //= LED6_OFF_H;

        // WW
        STRIP.WW_REG.ON_L = LED7_ON_L;   //= LED7_ON_L;
        STRIP.WW_REG.ON_H = LED7_ON_H;   //= LED7_ON_H;
        STRIP.WW_REG.OFF_L = LED7_OFF_L; //= LED7_OFF_L;
        STRIP.WW_REG.OFF_H = LED7_OFF_H; //= LED7_OFF_H;

        // RED
        STRIP.RED_REG.ON_L = LED5_ON_L;   //= LED3_ON_L;
        STRIP.RED_REG.ON_H = LED5_ON_H;   //= LED3_ON_H;
        STRIP.RED_REG.OFF_L = LED5_OFF_L; //= LED3_OFF_L;
        STRIP.RED_REG.OFF_H = LED5_OFF_H; //= LED3_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L = LED6_ON_L;   //= LED4_ON_L;
        STRIP.GREEN_REG.ON_H = LED6_ON_H;   //= LED4_ON_H;
        STRIP.GREEN_REG.OFF_L = LED6_OFF_L; //= LED4_OFF_L;
        STRIP.GREEN_REG.OFF_H = LED6_OFF_H; //= LED4_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L = LED4_ON_L;   //= LED5_ON_L;
        STRIP.BLUE_REG.ON_H = LED4_ON_H;   //= LED5_ON_H;
        STRIP.BLUE_REG.OFF_L = LED4_OFF_L; //= LED5_OFF_L;
        STRIP.BLUE_REG.OFF_H = LED4_OFF_H; //= LED5_OFF_H;

        return STRIP;
    }

    // LED Strip 2
    if (stripID == 2)
    {
        LEDStripColorReg STRIP;

        // CW
        STRIP.CW_REG.ON_L = LED8_ON_L;   //= LED1_ON_L;
        STRIP.CW_REG.ON_H = LED8_ON_H;   //= LED1_ON_H;
        STRIP.CW_REG.OFF_L = LED8_OFF_L; //= LED1_OFF_L;
        STRIP.CW_REG.OFF_H = LED8_OFF_H; //= LED1_OFF_H;

        // WW
        STRIP.WW_REG.ON_L = LED12_ON_L;   //= LED12_ON_L;
        STRIP.WW_REG.ON_H = LED12_ON_H;   //= LED12_ON_H;
        STRIP.WW_REG.OFF_L = LED12_OFF_L; //= LED12_OFF_L;
        STRIP.WW_REG.OFF_H = LED12_OFF_H; //= LED12_OFF_H;

        // RED
        STRIP.RED_REG.ON_L = LED10_ON_L;   //= LED8_ON_L;
        STRIP.RED_REG.ON_H = LED10_ON_H;   //= LED8_ON_H;
        STRIP.RED_REG.OFF_L = LED10_OFF_L; //= LED8_OFF_L;
        STRIP.RED_REG.OFF_H = LED10_OFF_H; //= LED8_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L = LED11_ON_L;   //= LED9_ON_L;
        STRIP.GREEN_REG.ON_H = LED11_ON_H;   //= LED9_ON_H;
        STRIP.GREEN_REG.OFF_L = LED11_OFF_L; //= LED9_OFF_L;
        STRIP.GREEN_REG.OFF_H = LED11_OFF_H; //= LED9_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L = LED9_ON_L;   //= LED10_ON_L;
        STRIP.BLUE_REG.ON_H = LED9_ON_H;   //= LED10_ON_H;
        STRIP.BLUE_REG.OFF_L = LED9_OFF_L; //= LED10_OFF_L;
        STRIP.BLUE_REG.OFF_H = LED9_OFF_H; //= LED10_OFF_H;

        return STRIP;
    }
};

/**
 * Prints all used register values from the PCA9685PW
 **/
/*
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
*/

/**
 * Prints a byte with leading zeros 
 * 
 * @parameter byte  The byte to print in binary
 **/
/*
void LedDriver::PrintByte(byte b)
{
    for (int i = 7; i >= 0; i--)
    {
        Serial.print(bitRead(b, i));
    }
    Serial.println("");
};
*/

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
