#pragma once

#include "Enums.h"

// Struct for time data
struct TimeData
{
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
};

// Struct for motion data
struct MotionData
{
    bool power = false;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint16_t timeout = 0;
};

// Struct for led strip data
struct LEDStripData
{
    bool power = false;
    uint16_t brightness = 0;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint8_t cw = 0;
    uint8_t ww = 0;
    LEDEffect effect = LEDEffect::None;
};
