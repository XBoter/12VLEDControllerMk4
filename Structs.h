#ifndef STRUCTS_H_INCLUDE
#define STRUCTS_H_INCLUDE

#include "Enums.h"

namespace LedControllerSoftwareMk5
{

    struct TimeData{
        uint8_t hour = 0;
        uint8_t minute = 0;
        uint8_t second = 0;
    };

    struct MotionData{
        bool power          = false;
        uint8_t red         = 0;
        uint8_t green       = 0;
        uint8_t blue        = 0;
        uint16_t timeout    = 0;
    };

    struct LEDStripData{
        bool power          = false;
        uint16_t brightness = 0;
        uint8_t red         = 0;
        uint8_t green       = 0;
        uint8_t blue        = 0;
        uint8_t cw          = 0;
        uint8_t ww          = 0;
        LEDEffect effect    = LEDEffect::None;
    };

};

#endif