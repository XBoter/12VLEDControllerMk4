#include "LedDriver.h"

/**
 * Constructor for the LedDriver class
 * 
 * @parameter i2cAddress    The i2c address of the pwm ic
 */
LedDriver::LedDriver(uint8_t i2cAddress)
{
    this->i2cAddress = i2cAddress;
};

/**
 * Sets reference to external components
 */
void LedDriver::setReference(I2C *i2c,
                             Network *network,
                             PirReader *pirReader)
{
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

        createInitalTypes();

        // ---- Calculate refresh rate data
        intervalRefreshRate = (unsigned long)(1000.0 / LED_STRIP_REFRESH_RATE);
        Serial.print(F("LED Strip get refreshed every '"));
        Serial.print(intervalRefreshRate);
        Serial.println(F("' Milliseconds"));

        Serial.println(F("LED Driver initialized"));

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

    // Refersh LED Strip data every x seconds => Needed for time based color fade
    unsigned long currentMillisRefreshRate = millis();
    if (currentMillisRefreshRate - previousMillisRefreshRate >= intervalRefreshRate)
    {
        previousMillisRefreshRate = currentMillisRefreshRate;
        refreshRateCounter++;

        // ---- Copy data from network to led driver
        networkMotionData = network->stNetworkMotionData;
        networkLEDStrip1Data = network->stNetworkLedStrip1Data;
        networkLEDStrip2Data = network->stNetworkLedStrip2Data;

        // Check if WiFi or MQTT got a disconnect and start the timer
        unsigned long CurMillis_ConnectionLost = millis();
        if(!network->wifiConnected || !network->mqttConnected)
        {
            if (CurMillis_ConnectionLost - PrevMillis_ConnectionLost >= TimeOut_ConnectionLost)
            {
                ConnectionLost = true;
            }
        }
        else
        {
            PrevMillis_ConnectionLost = CurMillis_ConnectionLost;
            ConnectionLost = false;
        }

        // Only Display when we got a connection
        if (!ConnectionLost)
        {
            // Wait a little to receive data from mqtt before showing led strip
            if (currentMillisRefreshRate - prevMillisReconnect >= timeoutReconnect)
            {
                // -- Handle multi LED strip effects
                HandleMultiLEDStripControlLogic();
                HandleMultiLEDStripEffects();
            }
        }
        else
        {
            prevMillisReconnect = currentMillisRefreshRate;
            FadeToBlack();
        }


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
 * Handles the logic control for multi led strip effects
 */
void LedDriver::HandleMultiLEDStripControlLogic()
{
    MultiLEDStripEffectData *effectData = getMultiLEDStripEffectData();

    if (network->alarm)
    {
        effectData->multiLEDEffect = MultiLEDEffect::Alarm;
    }
    else
    {
        if (network->parameter_master_present)
        {
            if (networkLEDStrip1Data.power || networkLEDStrip2Data.power)
            {
                effectData->multiLEDEffect = MultiLEDEffect::SingleLEDEffect;
            }
            else
            {
                if (pirReader->motionDetected && network->stNetworkMotionData.motionDetectionEnabled && network->parameter_sun)
                {
                    effectData->multiLEDEffect = MultiLEDEffect::MotionDetected;
                }
                else
                {
                    effectData->multiLEDEffect = MultiLEDEffect::Idle;
                }
            }
        }
        else
        {
            effectData->multiLEDEffect = MultiLEDEffect::Idle;
        }
    }
}

/**
 * Handels the display of multi LED strip effects
 */
void LedDriver::HandleMultiLEDStripEffects()
{
    bool fadeToBlackStrip1Finished = false;
    bool fadeToBlackStrip2Finished = false;
    bool fadeToColorStrip1Finished = false;
    bool fadeToColorStrip2Finished = false;

    // Get Effect data
    MultiLEDStripEffectData *effectData = getMultiLEDStripEffectData();
    HighLevelLEDStripData highLevelLEDStripData = getHighLevelFadeTimesAndCurves();

    // Check for effect change
    if (effectData->multiLEDEffect != prevMultiLEDEffect)
    {
        effectData->fadeFinished = false;
        effectData->counter = 0;
        effectData->transitionState = 0;
        effectData->subTransitionState = 0;
        prevMultiLEDEffect = effectData->multiLEDEffect;
    }

    // effect change state machine
    switch (effectData->transitionState)
    {

        // Fade both strips to black
    case 0:
        fadeToBlackStrip1Finished = FadeToBlack(1);
        fadeToBlackStrip2Finished = FadeToBlack(2);
        if (fadeToBlackStrip1Finished && fadeToBlackStrip2Finished)
        {
            effectData->prevMillis = millis();
            effectData->transitionState = 10;
        }
        break;

        // Display effect
    case 10:

        switch (effectData->multiLEDEffect)
        {

        case MultiLEDEffect::Idle:
            FadeToBlack(1);
            FadeToBlack(2);
            break;

        case MultiLEDEffect::SingleLEDEffect:
            HandleSingleLEDStripEffects(1, networkLEDStrip1Data);
            HandleSingleLEDStripEffects(2, networkLEDStrip2Data);
            break;

        case MultiLEDEffect::MotionDetected:
            highLevelLEDStripData.redColorValue = networkMotionData.redColorValue;
            highLevelLEDStripData.greenColorValue = networkMotionData.greenColorValue;
            highLevelLEDStripData.blueColorValue = networkMotionData.blueColorValue;
            highLevelLEDStripData.whiteTemperatureValue = networkMotionData.whiteTemperatureValue;

            // Check for timebased brightness
            if (networkMotionData.timeBasedBrightnessChangeEnabled)
            {
                // Map brightness value
                uint8_t percent = getMotionBrightnessPercent();
                highLevelLEDStripData.colorBrightnessValue = (uint16_t)((double)networkMotionData.colorBrightnessValue * ((double)percent / 100.0));
                highLevelLEDStripData.whiteBrightnessValue = (uint16_t)((double)networkMotionData.whiteBrightnessValue * ((double)percent / 100.0));
            }
            else
            {
                highLevelLEDStripData.colorBrightnessValue = networkMotionData.colorBrightnessValue;
                highLevelLEDStripData.whiteBrightnessValue = networkMotionData.whiteBrightnessValue;
            }
            FadeToColor(1, highLevelLEDStripData);
            FadeToColor(2, highLevelLEDStripData);
            break;

        case MultiLEDEffect::Alarm:

            highLevelLEDStripData.redColorValue = 255;
            highLevelLEDStripData.greenColorValue = 0;
            highLevelLEDStripData.blueColorValue = 0;
            highLevelLEDStripData.colorBrightnessValue = 4096;

            highLevelLEDStripData.whiteTemperatureValue = 0;
            highLevelLEDStripData.whiteBrightnessValue = 0;

            switch (effectData->subTransitionState)
            {

            case 0:
                fadeToBlackStrip1Finished = FadeToBlack(1);
                fadeToBlackStrip2Finished = FadeToBlack(2);
                if (fadeToBlackStrip1Finished && fadeToBlackStrip2Finished)
                {
                    effectData->prevMillis = millis();
                    effectData->subTransitionState = 10;
                }
                break;
            
            case 10:
                if(millis() - effectData->prevMillis >= 500){
                    effectData->subTransitionState = 20;
                }
                break;

            case 20:
                fadeToColorStrip1Finished = FadeToColor(1, highLevelLEDStripData);
                fadeToColorStrip2Finished = FadeToColor(2, highLevelLEDStripData);
                if (fadeToColorStrip1Finished && fadeToColorStrip2Finished)
                {
                    effectData->prevMillis = millis();
                    effectData->subTransitionState = 30;
                }
                break;

            case 30:
                if(millis() - effectData->prevMillis >= 1500){
                    effectData->subTransitionState = 0;
                }
                break;

            }

            break;
        }

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
    LowLevelLEDStripData lowLevelLEDStripData = getLowLevelFadeTimesAndCurves();

    // Check fo power
    if (commandNetworkLEDStripData.power)
    {

        // Check for effect change
        if (commandNetworkLEDStripData.effect != effectData->singleLEDEffect)
        {
            effectData->fadeFinished = false;
            effectData->counter = 0;
            effectData->transitionState = 0;
            effectData->subTransitionState = 0;
            effectData->singleLEDEffect = commandNetworkLEDStripData.effect;
        }

        // effect change state machine
        switch (effectData->transitionState)
        {

            // Fade Strip to black and set color
        case 0:
            if (FadeToBlack(stripID))
            {
                effectData->prevMillis = millis();
                effectData->transitionState = 10;
            }
            break;

            // Display effect
        case 10:

            switch (effectData->singleLEDEffect)
            {

            case SingleLEDEffect::None:
                FadeToColor(stripID, commandNetworkLEDStripData);
                break;

            case SingleLEDEffect::TriplePulse:
                // Red
                lowLevelLEDStripData.redColorValue = commandNetworkLEDStripData.ledStripData.redColorValue;
                // Green
                lowLevelLEDStripData.greenColorValue = commandNetworkLEDStripData.ledStripData.greenColorValue;
                //Blue
                lowLevelLEDStripData.blueColorValue = commandNetworkLEDStripData.ledStripData.blueColorValue;

                switch (effectData->subTransitionState)
                {
                // Fade to 12,5% brightness and wait 1500 milliseconds
                case 0:
                    // Red
                    lowLevelLEDStripData.redBrightnessValue = 512;
                    lowLevelLEDStripData.redBrightnessFadeTime = 800;
                    // Green
                    lowLevelLEDStripData.greenBrightnessValue = 512;
                    lowLevelLEDStripData.greenBrightnessFadeTime = 800;
                    // Blue
                    lowLevelLEDStripData.blueBrightnessValue = 512;
                    lowLevelLEDStripData.blueBrightnessFadeTime = 800;

                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        if (millis() - effectData->prevMillis >= 1500)
                        {
                            effectData->subTransitionState = 10;
                        }
                    }
                    else
                    {
                        effectData->prevMillis = millis();
                    }
                    break;

                // Fade to 100% brightness and wait 100 milliseconds
                case 10:
                    // Red
                    lowLevelLEDStripData.redBrightnessValue = 4096;
                    lowLevelLEDStripData.redBrightnessFadeTime = 400;
                    // Green
                    lowLevelLEDStripData.greenBrightnessValue = 4096;
                    lowLevelLEDStripData.greenBrightnessFadeTime = 400;
                    // Blue
                    lowLevelLEDStripData.blueBrightnessValue = 4096;
                    lowLevelLEDStripData.blueBrightnessFadeTime = 400;

                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        if (millis() - effectData->prevMillis >= 100)
                        {
                            effectData->subTransitionState = 20;
                        }
                    }
                    else
                    {
                        effectData->prevMillis = millis();
                    }
                    break;

                // Fade to 12,5% and check counter
                case 20:
                    // Red
                    lowLevelLEDStripData.redBrightnessValue = 512;
                    lowLevelLEDStripData.redBrightnessFadeTime = 400;
                    // Green
                    lowLevelLEDStripData.greenBrightnessValue = 512;
                    lowLevelLEDStripData.greenBrightnessFadeTime = 400;
                    // Blue
                    lowLevelLEDStripData.blueBrightnessValue = 512;
                    lowLevelLEDStripData.blueBrightnessFadeTime = 400;

                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        effectData->prevMillis = millis();
                        if (effectData->counter >= 2)
                        {
                            effectData->counter = 0;
                            effectData->subTransitionState = 0;
                        }
                        else
                        {
                            effectData->counter++;
                            effectData->subTransitionState = 30;
                        }
                    }
                    break;
                // Wait 200 milliseconds
                case 30:
                    // Red
                    lowLevelLEDStripData.redBrightnessValue = 512;
                    lowLevelLEDStripData.redBrightnessFadeTime = 400;
                    // Green
                    lowLevelLEDStripData.greenBrightnessValue = 512;
                    lowLevelLEDStripData.greenBrightnessFadeTime = 400;
                    // Blue
                    lowLevelLEDStripData.blueBrightnessValue = 512;
                    lowLevelLEDStripData.blueBrightnessFadeTime = 400;

                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        if (millis() - effectData->prevMillis >= 200)
                        {
                            effectData->subTransitionState = 10;
                        }
                    }
                    else
                    {
                        effectData->prevMillis = millis();
                    }
                    break;
                }

                break;

            case SingleLEDEffect::Rainbow:
                // Red
                lowLevelLEDStripData.redBrightnessValue = commandNetworkLEDStripData.ledStripData.colorBrightnessValue;
                lowLevelLEDStripData.redColorFadeTime = 6000;
                // Green
                lowLevelLEDStripData.greenBrightnessValue = commandNetworkLEDStripData.ledStripData.colorBrightnessValue;
                lowLevelLEDStripData.greenColorFadeTime = 6000;
                //Blue
                lowLevelLEDStripData.blueBrightnessValue = commandNetworkLEDStripData.ledStripData.colorBrightnessValue;
                lowLevelLEDStripData.blueColorFadeTime = 6000;

                switch (effectData->subTransitionState)
                {
                    // Fade to red
                case 0:
                    lowLevelLEDStripData.redColorValue = 255;
                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        effectData->subTransitionState = 10;
                    }
                    break;
                    // Fade to green
                case 10:
                    lowLevelLEDStripData.greenColorValue = 255;
                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
                    {
                        effectData->subTransitionState = 20;
                    }
                    break;
                    // Fade to blue
                case 20:
                    lowLevelLEDStripData.blueColorValue = 255;
                    effectData->fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
                    if (effectData->fadeFinished)
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
        FadeToBlack(stripID);
    }
};

/**
 * Returns a brightness percent value based on the current time
 * 
 * @return Brightness percent
 */
uint8_t LedDriver::getMotionBrightnessPercent()
{
    uint8_t percent = 100;

    if (stTimeBasedMotionBrightness.isSunfallSet && stTimeBasedMotionBrightness.isSunriseSet)
    {
        uint32_t difference = stTimeBasedMotionBrightness.sunfallUnix - stTimeBasedMotionBrightness.sunriseUnix;
        unsigned long lowestBrightnessUnix = stTimeBasedMotionBrightness.sunfallUnix + (unsigned long)((double)difference / 2.0);
        unsigned long maxBrightness = stTimeBasedMotionBrightness.sunfallUnix + difference;
        if (network->stNetworkTimeData.unix <= lowestBrightnessUnix)
        {
            percent = map(network->stNetworkTimeData.unix, stTimeBasedMotionBrightness.sunfallUnix, lowestBrightnessUnix, 100, 13);
        }
        else if (network->stNetworkTimeData.unix > lowestBrightnessUnix)
        {
            percent = map(network->stNetworkTimeData.unix, stTimeBasedMotionBrightness.sunfallUnix, maxBrightness, 13, 100);
        }
    }
    else
    {
        switch (network->stNetworkTimeData.hour)
        {
        // 100%
        case 20:
            percent = 100;
            break;
        // 75%
        case 21:
            percent = 75;
            break;
        // 50%
        case 22:
            percent = 50;
            break;
        // 37,5%
        case 23:
            percent = 38;
            break;
        // 25%
        case 24:
            percent = 25;
            break;
        // 12,5%
        case 0:
            percent = 13;
            break;
        // 12,5%
        case 1:
            percent = 13;
            break;
        // 12,5%
        case 2:
            percent = 13;
            break;
        // 12,5%
        case 3:
            percent = 13;
            break;
        // 25%
        case 4:
            percent = 25;
            break;
        // 37,5%
        case 5:
            percent = 38;
            break;
        // 50%
        case 6:
            percent = 50;
            break;
        // 75%
        case 7:
            percent = 75;
            break;
        // 100%
        case 8:
            percent = 100;
            break;
        }
    }

    return percent;
};

MultiLEDStripEffectData *LedDriver::getMultiLEDStripEffectData()
{
    return &multiLEDStripEffectData;
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
    switch (stripID)
    {
    case 1:
        return &singleLEDStrip1EffectData;
        break;

    case 2:
        return &singleLEDStrip2EffectData;
        break;

    default:
        return &emptySingleLEDStripEffectData;
        break;
    }
};

// # ================================================================ ================================================================ # //
// #                                                             DATA TYPES                                                            # //
// # ================================================================ ================================================================ # //

/**
 * Creates all defined default types
 */
void LedDriver::createInitalTypes()
{

    // ================================ HIGH LEVEL ================================ //
    // ======== defaultHighLevelFadeTimesAndFadeCurves
    defaultHighLevelFadeTimesAndFadeCurves.colorFadeTime = 1000; // Milliseconds
    defaultHighLevelFadeTimesAndFadeCurves.colorFadeCurve = FadeCurve::Linear;
    defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeTime = 800; // Milliseconds
    defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeCurve = FadeCurve::Linear;

    defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeTime = 1000; // Milliseconds
    defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeCurve = FadeCurve::Linear;
    defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeTime = 600; // Milliseconds       // White channel led types needs longer to fade out so we short the fade time to match it with the rgb channel led types
    defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeCurve = FadeCurve::Linear;

    // ======== instantHighLevelFadeTimesAndCurves
    instantHighLevelFadeTimesAndCurves.colorFadeTime = 0;
    instantHighLevelFadeTimesAndCurves.colorFadeCurve = FadeCurve::None;
    instantHighLevelFadeTimesAndCurves.colorBrightnessFadeTime = 0;
    instantHighLevelFadeTimesAndCurves.colorBrightnessFadeCurve = FadeCurve::None;
    instantHighLevelFadeTimesAndCurves.whiteTemperatureFadeTime = 0;
    instantHighLevelFadeTimesAndCurves.whiteTemperatureFadeCurve = FadeCurve::None;
    instantHighLevelFadeTimesAndCurves.whiteBrightnessFadeTime = 0;
    instantHighLevelFadeTimesAndCurves.whiteBrightnessFadeCurve = FadeCurve::None;

    // ================================ LOW LEVEL ================================ //
    // ======== defaultLowLevelFadeTimesAndFadeCurves
    // ---- Color
    // -- RED
    defaultLowLevelFadeTimesAndFadeCurves.redColorFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.redColorFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorFadeCurve;
    defaultLowLevelFadeTimesAndFadeCurves.redBrightnessFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.redBrightnessFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeCurve;
    // -- GREEN
    defaultLowLevelFadeTimesAndFadeCurves.greenColorFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.greenColorFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorFadeCurve;
    defaultLowLevelFadeTimesAndFadeCurves.greenBrightnessFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.greenBrightnessFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeCurve;
    // -- BLUE
    defaultLowLevelFadeTimesAndFadeCurves.blueColorFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.blueColorFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorFadeCurve;
    defaultLowLevelFadeTimesAndFadeCurves.blueBrightnessFadeTime = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.blueBrightnessFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.colorBrightnessFadeCurve;

    // ---- White
    // -- CW
    defaultLowLevelFadeTimesAndFadeCurves.cwColorFadeTime = defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.cwColorFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeCurve;
    defaultLowLevelFadeTimesAndFadeCurves.cwBrightnessFadeTime = defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.cwBrightnessFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeCurve;
    // -- WW
    defaultLowLevelFadeTimesAndFadeCurves.wwColorFadeTime = defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.wwColorFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.whiteTemperatureFadeCurve;
    defaultLowLevelFadeTimesAndFadeCurves.wwBrightnessFadeTime = defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeTime;
    defaultLowLevelFadeTimesAndFadeCurves.wwBrightnessFadeCurve = defaultHighLevelFadeTimesAndFadeCurves.whiteBrightnessFadeCurve;

    // ======== instantLowLevelFadeTimesAndCurves
    // ---- Color
    // -- RED
    instantLowLevelFadeTimesAndCurves.redColorFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.redColorFadeCurve = FadeCurve::None;
    instantLowLevelFadeTimesAndCurves.redBrightnessFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.redBrightnessFadeCurve = FadeCurve::None;
    // -- GREEN
    instantLowLevelFadeTimesAndCurves.greenColorFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.greenColorFadeCurve = FadeCurve::None;
    instantLowLevelFadeTimesAndCurves.greenBrightnessFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.greenBrightnessFadeCurve = FadeCurve::None;
    // -- BLUE
    instantLowLevelFadeTimesAndCurves.blueColorFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.blueColorFadeCurve = FadeCurve::None;
    instantLowLevelFadeTimesAndCurves.blueBrightnessFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.blueBrightnessFadeCurve = FadeCurve::None;

    // ---- White
    // -- CW
    instantLowLevelFadeTimesAndCurves.cwColorFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.cwColorFadeCurve = FadeCurve::None;
    instantLowLevelFadeTimesAndCurves.cwBrightnessFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.cwBrightnessFadeCurve = FadeCurve::None;
    // -- WW
    instantLowLevelFadeTimesAndCurves.wwColorFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.wwColorFadeCurve = FadeCurve::None;
    instantLowLevelFadeTimesAndCurves.wwBrightnessFadeTime = 0;
    instantLowLevelFadeTimesAndCurves.wwBrightnessFadeCurve = FadeCurve::None;

    // ================================ SET DEFAULT TYPES ================================ //
    /*
        We set the default times for the current until they get override by network data
    */
    currentHighLevelFadeTimesAndFadeCurves = defaultHighLevelFadeTimesAndFadeCurves;
    currentLowLevelFadeTimesAndFadeCurves = defaultLowLevelFadeTimesAndFadeCurves;
}

/**
 * Returns a HighLevelLEDStripData with the current fade times and fade curves for all channels
 * 
 * @return current HighLevelLEDStripData
 */
HighLevelLEDStripData LedDriver::getHighLevelFadeTimesAndCurves()
{
    return currentHighLevelFadeTimesAndFadeCurves;
};

/**
 * Returns a LowLevelLEDStripData with the current fade times and fade curves for all channels
 * 
 * @return current LowLevelLEDStripData
 */
LowLevelLEDStripData LedDriver::getLowLevelFadeTimesAndCurves()
{
    return currentLowLevelFadeTimesAndFadeCurves;
};

/**
 * Returns a HighLevelLEDStripData with the instant fade times and fade curves for all channels
 * 
 * @return instant HighLevelLEDStripData
 */
HighLevelLEDStripData LedDriver::getInstantHighLevelFadeTimesAndCurves()
{
    return instantHighLevelFadeTimesAndCurves;
};

/**
 * Returns a LowLevelLEDStripData with the instant fade times and fade curves for all channels
 * 
 * @return instant LowLevelLEDStripData
 */
LowLevelLEDStripData LedDriver::getInstantLowLevelFadeTimesAndCurves()
{
    return instantLowLevelFadeTimesAndCurves;
};

/**
 * Converts NetworkLEDStripData to HighLevelLEDStripData and adds missing values
 * 
 * @return Converted HighLevelLEDStripData
 */
HighLevelLEDStripData LedDriver::convertNetworkDataToHighLevelData(NetworkLEDStripData networkData,
                                                                   HighLevelLEDStripData highLevelFadeTimesAndCurves)
{
    HighLevelLEDStripData highLevelLEDStripData = highLevelFadeTimesAndCurves;

    // Color Data
    highLevelLEDStripData.redColorValue = networkData.ledStripData.redColorValue;
    highLevelLEDStripData.greenColorValue = networkData.ledStripData.greenColorValue;
    highLevelLEDStripData.blueColorValue = networkData.ledStripData.blueColorValue;
    highLevelLEDStripData.colorBrightnessValue = networkData.ledStripData.colorBrightnessValue;

    highLevelLEDStripData.whiteTemperatureValue = networkData.ledStripData.whiteTemperatureValue;
    highLevelLEDStripData.whiteBrightnessValue = networkData.ledStripData.whiteBrightnessValue;

    return highLevelLEDStripData;
}

/**
 * Converts HighLevelLEDStripData to LowLevelLEDStripData and adds missing values
 * 
 * @return Converted LowLevelLEDStripData
 */
LowLevelLEDStripData LedDriver::convertHighLevelDataToLowLevelData(HighLevelLEDStripData highLevelLEDStripData,
                                                                   LowLevelLEDStripData lowLevelFadeTimesAndCurves)
{
    LowLevelLEDStripData lowLevelLEDStripData = lowLevelFadeTimesAndCurves;

    // Color Data
    lowLevelLEDStripData.redColorValue = highLevelLEDStripData.redColorValue;
    lowLevelLEDStripData.redBrightnessValue = highLevelLEDStripData.colorBrightnessValue;

    lowLevelLEDStripData.greenColorValue = highLevelLEDStripData.greenColorValue;
    lowLevelLEDStripData.greenBrightnessValue = highLevelLEDStripData.colorBrightnessValue;

    lowLevelLEDStripData.blueColorValue = highLevelLEDStripData.blueColorValue;
    lowLevelLEDStripData.blueBrightnessValue = highLevelLEDStripData.colorBrightnessValue;

    if (highLevelLEDStripData.whiteTemperatureValue >= 250)
    {

        lowLevelLEDStripData.cwColorValue = map(highLevelLEDStripData.whiteTemperatureValue, 500, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.cwColorValue = 255;
    }
    lowLevelLEDStripData.cwBrightnessValue = highLevelLEDStripData.whiteBrightnessValue;

    if (highLevelLEDStripData.whiteTemperatureValue <= 250)
    {
        lowLevelLEDStripData.wwColorValue = map(highLevelLEDStripData.whiteTemperatureValue, 1, 250, 0, 255);
    }
    else
    {
        lowLevelLEDStripData.wwColorValue = 255;
    }
    lowLevelLEDStripData.wwBrightnessValue = highLevelLEDStripData.whiteBrightnessValue;

    return lowLevelLEDStripData;
}

/**
 * Combines LowLevelLEDStripData(Color) and LowLevelLEDStripData(FadeTimes and FadeCurves) 
 * 
 * @return Combined LowLevelLEDStripData
 */
LowLevelLEDStripData LedDriver::combineLowLevelDataToLowLevelData(LowLevelLEDStripData lowLevelLEDStripData,
                                                                  LowLevelLEDStripData lowLevelFadeTimesAndCurves)
{
    // ==== Color
    // == RED
    lowLevelFadeTimesAndCurves.redColorValue = lowLevelLEDStripData.redColorValue;
    lowLevelFadeTimesAndCurves.redBrightnessValue = lowLevelLEDStripData.redBrightnessValue;
    // == GREEN
    lowLevelFadeTimesAndCurves.greenColorValue = lowLevelLEDStripData.greenColorValue;
    lowLevelFadeTimesAndCurves.greenBrightnessValue = lowLevelLEDStripData.greenBrightnessValue;
    // == BLUE
    lowLevelFadeTimesAndCurves.blueColorValue = lowLevelLEDStripData.blueColorValue;
    lowLevelFadeTimesAndCurves.blueBrightnessValue = lowLevelLEDStripData.blueBrightnessValue;

    // ==== White
    // == CW
    lowLevelFadeTimesAndCurves.cwColorValue = lowLevelLEDStripData.cwColorValue;
    lowLevelFadeTimesAndCurves.cwBrightnessValue = lowLevelLEDStripData.cwBrightnessValue;
    // == WW
    lowLevelFadeTimesAndCurves.wwColorValue = lowLevelLEDStripData.wwColorValue;
    lowLevelFadeTimesAndCurves.wwBrightnessValue = lowLevelLEDStripData.wwBrightnessValue;

    return lowLevelFadeTimesAndCurves;
}

// # ================================================================ ================================================================ # //
// #                                                             SET COLOR                                                             # //
// # ================================================================ ================================================================ # //

bool LedDriver::SetColor(uint8_t stripID,
                         NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeFinished = FadeToColor(stripID, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getInstantHighLevelFadeTimesAndCurves()));
    return fadeFinished;
};

bool LedDriver::SetColor(uint8_t stripID,
                         HighLevelLEDStripData commandHighLevelLEDStripData)
{
    bool fadeFinished = FadeToColor(stripID, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
    return fadeFinished;
};

bool LedDriver::SetColor(uint8_t stripID,
                         LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeFinished = FadeToColor(stripID, combineLowLevelDataToLowLevelData(commandLowLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
    return fadeFinished;
};

bool LedDriver::SetColor(NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getInstantHighLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getInstantHighLevelFadeTimesAndCurves()));
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

bool LedDriver::SetColor(HighLevelLEDStripData commandHighLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

bool LedDriver::SetColor(LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, combineLowLevelDataToLowLevelData(commandLowLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, combineLowLevelDataToLowLevelData(commandLowLevelLEDStripData, getInstantLowLevelFadeTimesAndCurves()));
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
    bool fadeFinished = FadeToColor(stripID, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getHighLevelFadeTimesAndCurves()));
    return fadeFinished;
};

bool LedDriver::FadeToColor(uint8_t stripID,
                            HighLevelLEDStripData commandHighLevelLEDStripData)
{
    bool fadeFinished = FadeToColor(stripID, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getLowLevelFadeTimesAndCurves()));
    return fadeFinished;
};

bool LedDriver::FadeToColor(uint8_t stripID,
                            LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeFinished = true;
    unsigned long curMillis = previousMillisRefreshRate;

    // Get current data of strip
    LEDStripData *ptrCurrentLEDStripData = getCurrentLEDStripData(stripID);

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
        ptrCurrentLEDStripData->redColorValue = getCurveValue(commandLowLevelLEDStripData.redColorFadeCurve,
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
        ptrCurrentLEDStripData->greenColorValue = getCurveValue(commandLowLevelLEDStripData.greenColorFadeCurve,
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
        ptrCurrentLEDStripData->blueColorValue = getCurveValue(commandLowLevelLEDStripData.blueColorFadeCurve,
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
        ptrCurrentLEDStripData->cwColorValue = getCurveValue(commandLowLevelLEDStripData.cwColorFadeCurve,
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
        ptrCurrentLEDStripData->wwColorValue = getCurveValue(commandLowLevelLEDStripData.wwColorFadeCurve,
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

bool LedDriver::FadeToColor(NetworkLEDStripData commandNetworkLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getHighLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, convertNetworkDataToHighLevelData(commandNetworkLEDStripData, getHighLevelFadeTimesAndCurves()));
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

bool LedDriver::FadeToColor(HighLevelLEDStripData commandHighLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getLowLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, convertHighLevelDataToLowLevelData(commandHighLevelLEDStripData, getLowLevelFadeTimesAndCurves()));
    if (fadeStrip1Finished && fadeStrip2Finished)
    {
        return true;
    }
    else
    {
        return false;
    }
};

bool LedDriver::FadeToColor(LowLevelLEDStripData commandLowLevelLEDStripData)
{
    bool fadeStrip1Finished = FadeToColor(1, combineLowLevelDataToLowLevelData(commandLowLevelLEDStripData, getLowLevelFadeTimesAndCurves()));
    bool fadeStrip2Finished = FadeToColor(2, combineLowLevelDataToLowLevelData(commandLowLevelLEDStripData, getLowLevelFadeTimesAndCurves()));
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

bool LedDriver::FadeToBlack(uint8_t stripID)
{
    LowLevelLEDStripData lowLevelLEDStripData = getLowLevelFadeTimesAndCurves();

    lowLevelLEDStripData.redBrightnessValue = 0;
    lowLevelLEDStripData.greenBrightnessValue = 0;
    lowLevelLEDStripData.blueBrightnessValue = 0;
    lowLevelLEDStripData.cwBrightnessValue = 0;
    lowLevelLEDStripData.wwBrightnessValue = 0;

    bool fadeFinished = FadeToColor(stripID, lowLevelLEDStripData);
    return fadeFinished;
};

bool LedDriver::FadeToBlack()
{
    LowLevelLEDStripData lowLevelLEDStripData = getLowLevelFadeTimesAndCurves();

    lowLevelLEDStripData.redBrightnessValue = 0;
    lowLevelLEDStripData.greenBrightnessValue = 0;
    lowLevelLEDStripData.blueBrightnessValue = 0;
    lowLevelLEDStripData.cwBrightnessValue = 0;
    lowLevelLEDStripData.wwBrightnessValue = 0;

    bool fadeStrip1Finished = FadeToColor(1, lowLevelLEDStripData);
    bool fadeStrip2Finished = FadeToColor(2, lowLevelLEDStripData);
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
    uint16_t value = 0;

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
        value = end;
        break;

    case FadeCurve::Linear:
        value = linear(percent,
                       start,
                       end);
        break;

    case FadeCurve::EaseIn:
        value = easeInQuart(percent,
                            start,
                            end);
        break;

    case FadeCurve::EaseOut:
        value = easeOutQuart(percent,
                             start,
                             end);
        break;

    case FadeCurve::EaseInOut:
        value = easeInOutQuart(percent,
                               start,
                               end);
        break;

    default:
        value = end;
        break;
    }

    return value;
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
    switch (stripID)
    {
    case 1:
        return &currentLEDStrip1Data;
        break;

    case 2:
        return &currentLEDStrip2Data;
        break;

    default:
        return &emptyCurrentLEDStripData;
        break;
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
    switch (stripID)
    {
    case 1:
        return &prevLEDStrip1Data;
        break;

    case 2:
        return &prevLEDStrip2Data;
        break;

    default:
        return &emptyPrevLEDStripData;
        break;
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
    LEDStripColorReg STRIP = {};

    switch (stripID)
    {
    case 1:
        // CW
        STRIP.CW_REG.ON_L = LED3_ON_L;
        STRIP.CW_REG.ON_H = LED3_ON_H;
        STRIP.CW_REG.OFF_L = LED3_OFF_L;
        STRIP.CW_REG.OFF_H = LED3_OFF_H;

        // WW
        STRIP.WW_REG.ON_L = LED7_ON_L;
        STRIP.WW_REG.ON_H = LED7_ON_H;
        STRIP.WW_REG.OFF_L = LED7_OFF_L;
        STRIP.WW_REG.OFF_H = LED7_OFF_H;

        // RED
        STRIP.RED_REG.ON_L = LED5_ON_L;
        STRIP.RED_REG.ON_H = LED5_ON_H;
        STRIP.RED_REG.OFF_L = LED5_OFF_L;
        STRIP.RED_REG.OFF_H = LED5_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L = LED6_ON_L;
        STRIP.GREEN_REG.ON_H = LED6_ON_H;
        STRIP.GREEN_REG.OFF_L = LED6_OFF_L;
        STRIP.GREEN_REG.OFF_H = LED6_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L = LED4_ON_L;
        STRIP.BLUE_REG.ON_H = LED4_ON_H;
        STRIP.BLUE_REG.OFF_L = LED4_OFF_L;
        STRIP.BLUE_REG.OFF_H = LED4_OFF_H;

        return STRIP;
        break;

    case 2:
        // CW
        STRIP.CW_REG.ON_L = LED8_ON_L;
        STRIP.CW_REG.ON_H = LED8_ON_H;
        STRIP.CW_REG.OFF_L = LED8_OFF_L;
        STRIP.CW_REG.OFF_H = LED8_OFF_H;

        // WW
        STRIP.WW_REG.ON_L = LED12_ON_L;
        STRIP.WW_REG.ON_H = LED12_ON_H;
        STRIP.WW_REG.OFF_L = LED12_OFF_L;
        STRIP.WW_REG.OFF_H = LED12_OFF_H;

        // RED
        STRIP.RED_REG.ON_L = LED10_ON_L;
        STRIP.RED_REG.ON_H = LED10_ON_H;
        STRIP.RED_REG.OFF_L = LED10_OFF_L;
        STRIP.RED_REG.OFF_H = LED10_OFF_H;

        // GREEN
        STRIP.GREEN_REG.ON_L = LED11_ON_L;
        STRIP.GREEN_REG.ON_H = LED11_ON_H;
        STRIP.GREEN_REG.OFF_L = LED11_OFF_L;
        STRIP.GREEN_REG.OFF_H = LED11_OFF_H;

        // BLUE
        STRIP.BLUE_REG.ON_L = LED9_ON_L;
        STRIP.BLUE_REG.ON_H = LED9_ON_H;
        STRIP.BLUE_REG.OFF_L = LED9_OFF_L;
        STRIP.BLUE_REG.OFF_H = LED9_OFF_H;

        return STRIP;
        break;

    default:
        return STRIP;
        break;
    }
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

/**
 * LED driver configuration mode. Turns off all led strips.
 */
bool LedDriver::ConfigureMode()
{
    bool finishedConfigureMode = false;

    // Check for init
    if (!init)
    {
        Init();
    }
    else
    {

        unsigned long currentMillisRefreshRate = millis();
        if (currentMillisRefreshRate - previousMillisRefreshRate >= intervalRefreshRate)
        {
            previousMillisRefreshRate = currentMillisRefreshRate;
            refreshRateCounter++;

            // Fade both strips to black
            finishedConfigureMode = FadeToBlack();

            // Call update LED Strips because LED Driver gets not called in configuration mode
            UpdateLEDStrip(1);
            UpdateLEDStrip(2);
        }
    }

    return finishedConfigureMode;
}