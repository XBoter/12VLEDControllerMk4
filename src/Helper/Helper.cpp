#include "Helper.h"

/**
 * @brief Construct a new Helper:: Helper object
 * 
 */
Helper::Helper(){

};

/**
 * @brief Sets the needed refernce for the helper
 */
void Helper::setReference(){};

/**
 * @brief Initializes the helper component
 * 
 * @return True if the initialization was successful
 */
bool Helper::Init()
{
    if (!init)
    {

        init = true;
    }
    return init;
};

/**
 * @brief Runs the helper component
 * 
 */
void Helper::Run()
{
    if (!init)
    {
        return;
    }
};

/**
 * @brief Converts the ENUM LEDOutputType to an uint8_t value
 * 
 * @param value The ENUM LEDOutputType value
 * @return The converted value
 */
uint8_t Helper::convertLEDOutputTypeToUint8(LEDOutputType value)
{
    uint8_t result = 0;
    switch (value)
    {
    case LEDOutputType::R:
        result = 0;
        break;
    case LEDOutputType::G:
        result = 1;
        break;
    case LEDOutputType::B:
        result = 2;
        break;
    case LEDOutputType::CW:
        result = 3;
        break;
    case LEDOutputType::WW:
        result = 4;
        break;
    }
    return result;
}

/**
 * @brief Converts the uint8_t value to an ENUM LEDOutputType
 * 
 * @param value The uint8_t ENUM value
 * @return The converted value
 */
LEDOutputType Helper::convertUint8ToLEDOutputType(uint8_t value)
{
    LEDOutputType result = LEDOutputType::R;
    switch (value)
    {
    case 0:
        result = LEDOutputType::R;
        break;
    case 1:
        result = LEDOutputType::G;
        break;
    case 2:
        result = LEDOutputType::B;
        break;
    case 3:
        result = LEDOutputType::CW;
        break;
    case 4:
        result = LEDOutputType::WW;
        break;
    }
    return result;
}

/**
 * @brief Converts the ENUM SingleLEDEffect to an uint8_t value
 * 
 * @param value The ENUM SingleLEDEffect value
 * @return The converted value
 */
uint8_t Helper::convertSingleLEDEffectToUint8(SingleLEDEffect value)
{
    uint8_t result = 0;
    switch (value)
    {
    case SingleLEDEffect::None:
        result = 0;
        break;
    case SingleLEDEffect::Rainbow:
        result = 1;
        break;
    case SingleLEDEffect::TriplePulse:
        result = 2;
    }
    return result;
}

/**
 * @brief Converts the uint8_t value to an ENUM SingleLEDEffect
 * 
 * @param value The uint8_t ENUM value
 * @return The converted value
 */
SingleLEDEffect Helper::convertUint8ToSingleLEDEffect(uint8_t value)
{
    SingleLEDEffect result = SingleLEDEffect::None;
    switch (value)
    {
    case 0:
        result = SingleLEDEffect::None;
        break;
    case 1:
        result = SingleLEDEffect::Rainbow;
        break;
    case 2:
        result = SingleLEDEffect::TriplePulse;
        break;
    }
    return result;
}