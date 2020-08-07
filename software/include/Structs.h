#pragma once

#include "Enums.h"

// Struct for time data
struct NetworkTimeData
{
    uint8_t hour    = 0;
    uint8_t minute  = 0;
    uint8_t second  = 0;
};

// Struct for motion data
struct NetworkMotionData
{
    bool power              = false;
    uint8_t redColorValue   = 0;
    uint8_t greenColorValue = 0;
    uint8_t blueColorValue  = 0;
    uint16_t timeout        = 0;
};


/**
 * Holds all register for one LED Color
 */
struct LEDColorReg
{
    uint8_t ON_L    = 0x00;
    uint8_t ON_H    = 0x00;
    uint8_t OFF_L    = 0x00;
    uint8_t OFF_H   = 0x00;
};


/**
 * Holds all register for one LED strip
 */
struct LEDStripColorReg
{
    LEDColorReg RED_REG;
    LEDColorReg GREEN_REG;
    LEDColorReg BLUE_REG;
    LEDColorReg CW_REG;
    LEDColorReg WW_REG;
};


/**
 * A high level struct for the color channels with simple settings
 * For all channel ther is:
 *   - Color Fade Time          => The time it takes in millis to fade to the new value
 *   - Color Fade Curve         => The curve with which the color gets faded to the new value
 *   - Brightness Value         => One brightness value for all color channels
 *   - Brightness Fade Time     => The time it takes in millis to fade to new value
 *   - Brightness Fade Curve    => The curve with which the brightness gets faded to the new value
 */
struct HighLevelLEDStripData
{
    // ---- COLOR
    uint8_t redColorValue           = 0;
    uint8_t greenColorValue         = 0;
    uint8_t blueColorValue          = 0;
    uint8_t cwColorValue            = 0;
    uint8_t wwColorValue            = 0;
    uint16_t colorFadeTime          = 0;
    FadeCurve colorFadeCurve        = FadeCurve::None;

    // ---- BRIGHTNESS
    uint16_t brightnessValue        = 0;
    uint16_t brightnessFadeTime     = 0;
    FadeCurve brightnessFadeCurve   = FadeCurve::None;

};


/**
 * Struct with data for LED effets
 */ 
struct LEDEffectData
{
    LEDEffect prevEffect = LEDEffect::None;
    uint8_t transitionState = 0;
};


/**
 * The LED strip data received via mqtt
 */
struct NetworkLEDStripData
{
    bool power = false;
    HighLevelLEDStripData ledStripData;
    ControllerMode mode = ControllerMode::Idle;
    LEDEffect effect = LEDEffect::None;
};

/**
 * Holds data about the individual color channels of a RGB/CW/WW LED strip
 * For each channel there is:
 *   - Color Value              => The value of the color channel
 *   - Color Fade Time          => The time it takes in millis to fade to the new value
 *   - Color Fade Curve         => The curve with which the color gets faded to the new value
 *   - Brightness Value         => The correspoding brightness of that color channel
 *   - Brightness Fade Time     => The time it takes in millis to fade to the new value
 *   - Brightness Fade Curve    => The curve with which the brightness gets faded to the new value
 */
struct LowLevelLEDStripData
{
    // ---- RED
    // -- Color
    uint8_t redColorValue               = 0;
    uint16_t redColorFadeTime           = 0;
    FadeCurve redColorFadeCurve         = FadeCurve::None;
    // -- Brightness
    uint16_t redBrightnessValue         = 0;
    uint16_t redBrightnessFadeTime      = 0;
    FadeCurve redBrightnessFadeCurve    = FadeCurve::None;

    // ---- GREEN
    // -- Color
    uint8_t greenColorValue             = 0;
    uint16_t greenColorFadeTime         = 0;
    FadeCurve greenColorFadeCurve       = FadeCurve::None;
    // -- Brightness
    uint16_t greenBrightnessValue       = 0;
    uint16_t greenBrightnessFadeTime    = 0;
    FadeCurve greenBrightnessFadeCurve  = FadeCurve::None;

    // ---- BLUE
    // -- Color
    uint8_t blueColorValue              = 0;
    uint16_t blueColorFadeTime          = 0;
    FadeCurve blueColorFadeCurve        = FadeCurve::None;
    // -- Brightness
    uint16_t blueBrightnessValue        = 0;
    uint16_t blueBrightnessFadeTime     = 0;
    FadeCurve blueBrightnessFadeCurve   = FadeCurve::None;

    // ---- CW
    // -- Color
    uint8_t cwColorValue                = 0;
    uint16_t cwColorFadeTime            = 0;
    FadeCurve cwColorFadeCurve          = FadeCurve::None;
    // -- Brightness
    uint16_t cwBrightnessValue          = 0;
    uint16_t cwBrightnessFadeTime       = 0;
    FadeCurve cwBrightnessFadeCurve     = FadeCurve::None;

    // ---- WW
    // -- Color
    uint8_t wwColorValue                = 0;
    uint16_t wwColorFadeTime            = 0;
    FadeCurve wwColorFadeCurve          = FadeCurve::None;
    // -- Brightness
    uint16_t wwBrightnessValue          = 0;
    uint16_t wwBrightnessFadeTime       = 0;
    FadeCurve wwBrightnessFadeCurve     = FadeCurve::None;

};


/**
 * Hold the actual values of the LED strip and prev fade times
 */
struct LEDStripData
{
    // ---- RED
    // -- Color
    uint8_t redColorValue                       = 0;
    uint8_t prevRedColorValue                   = 0;
    unsigned long prevMillisRedColorFade        = 0;
    // -- Brightness
    uint16_t redBrightnessValue                 = 0;
    uint16_t prevRedBrightnessValue             = 0;
    unsigned long prevMillisRedBrightnessFade   = 0;

    // ---- GREEN
    // -- Color
    uint8_t greenColorValue                     = 0;
    uint8_t prevGreenColorValue                 = 0;
    unsigned long prevMillisGreenColorFade      = 0;
    // -- Brightness
    uint16_t greenBrightnessValue               = 0;
    uint16_t prevGreenBrightnessValue           = 0;
    unsigned long prevMillisGreenBrightnessFade = 0;

    // ---- BLUE
    // -- Color
    uint8_t blueColorValue                      = 0;
    uint8_t prevBlueColorValue                  = 0;
    unsigned long prevMillisBlueColorFade       = 0;
    // -- Brightness
    uint16_t blueBrightnessValue                = 0;
    uint16_t prevBlueBrightnessValue            = 0;
    unsigned long prevMillisBlueBrightnessFade  = 0;

    // ---- CW
    // -- Color
    uint8_t cwColorValue                        = 0;
    uint8_t prevCwColorValue                    = 0;
    unsigned long prevMillisCwColorFade         = 0;
    // -- Brightness
    uint16_t cwBrightnessValue                  = 0;
    uint16_t prevCwBrightnessValue              = 0;
    unsigned long prevMillisCwBrightnessFade    = 0;

    // ---- WW
    // -- Color
    uint8_t wwColorValue                        = 0;
    uint8_t prevWwColorValue                    = 0;
    unsigned long prevMillisWwColorFade         = 0;
    // -- Brightness
    uint16_t wwBrightnessValue                  = 0;
    uint16_t prevWwBrightnessValue              = 0;
    unsigned long prevMillisWwBrightnessFade    = 0;

};
