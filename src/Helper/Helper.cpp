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
        Serial.println(F("Helper initialized"));
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
 * @brief Converts the uint8_t value to an ENUM LEDOutputType
 * 
 * @param value The uint8_t ENUM value
 * @return The converted value
 */
LEDOutputType Helper::Uint8ToLEDOutputType(uint8_t value)
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
uint8_t Helper::SingleLEDEffectToUint8(SingleLEDEffect value)
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
SingleLEDEffect Helper::Uint8ToSingleLEDEffect(uint8_t value)
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

/**
 * @brief Prints a formatted top spacer message to serial
 * 
 */
void Helper::TopSpacerPrint()
{
    Serial.println(F(""));
    Serial.print(F(" # "));
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(F(" #"));
    Serial.println(F(""));
};

/**
 * @brief Prints a formatted bottom spacer message to serial
 * 
 */
void Helper::BottomSpacerPrint()
{
    Serial.print(F(" # "));
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(F(" #"));
    Serial.println(F(""));
};

/**
 * @brief Converts a given bool value to the corresponding string message
 * 
 * @param b The bool value to convert
 * @return The value of the bool as string 
 */
String Helper::BoolToString(bool b)
{
    if (b)
    {
        return "true";
    }
    else
    {
        return "false";
    }
};

/**
 * @brief Converts a given bool value to the corresponding connection state
 * 
 * @param b The bool value to convert
 * @return The value of the bool as connection state 
 */
String Helper::BollToConnectionState(bool b)
{
    if (b)
    {
        return "Connected";
    }
    else
    {
        return "Disconnected";
    }
};

/**
 * @brief Converts a given int value to the corresponding client state info
 * 
 * @param i The int value to convert
 * @return The value of the int as client state info 
 */
String Helper::IntToClientStateInfo(int i)
{
    switch (i)
    {

    case -4:
        return "Connection Timeout";

    case -3:
        return "Connection Lost";

    case -2:
        return "Connect Failed";

    case -1:
        return "Disconnected";

    case 0:
        return "Connected";

    case 1:
        return "Connect Bad Protocol";

    case 2:
        return "Connect Bad Client ID";

    case 3:
        return "Connect Unavailable";

    case 4:
        return "Connect Bad Credentials";

    case 5:
        return "Connect Unauthorized";

    default:
        return "Unknown";
    }
};

/**
 * @brief Converts a SingleLEDEffect to a String
 * 
 * @param effect The SingleLEDEffect to convert to string
 * @return The corresponding string effect to the given SingleLEDEffect 
 */
String Helper::SingleLEDEffectToString(SingleLEDEffect effect)
{
    switch (effect)
    {

    case SingleLEDEffect::None:
        return "None";
        break;

    case SingleLEDEffect::TriplePulse:
        return "TriplePulse";
        break;

    case SingleLEDEffect::Rainbow:
        return "Rainbow";
        break;

    default:
        return "None";
        break;
    }
};

/**
 * @brief Converts a string to a LEDEffect
 * 
 * @param effect The name of effect as string
 * @return The corresponding LEDEffect to the given string effect 
 */
SingleLEDEffect Helper::StringToSingleLEDEffect(String effect)
{
    if (effect == "None")
    {
        return SingleLEDEffect::None;
    }
    else if (effect == "TriplePulse")
    {
        return SingleLEDEffect::TriplePulse;
    }
    else if (effect == "Rainbow")
    {
        return SingleLEDEffect::Rainbow;
    }
    else // default
    {
        return SingleLEDEffect::None;
    }
};

/**
 * @brief Converts a MultiLEDEffect to a String
 * 
 * @param effect The MultiLEDEffect to convert to string
 * @return The corresponding string effect to the given MultiLEDEffect 
 */
String Helper::MultiLEDEffectToString(MultiLEDEffect effect)
{
    switch (effect)
    {

    case MultiLEDEffect::Idle:
        return "Idle";
        break;

    case MultiLEDEffect::SingleLEDEffect:
        return "SingleLEDEffect";
        break;

    case MultiLEDEffect::MotionDetected:
        return "MotionDetected";
        break;

    case MultiLEDEffect::Alarm:
        return "Alarm";
        break;

    default:
        return "Idle";
        break;
    }
};

/**
 * @brief Converts a FadeCurve to a String
 * 
 * @param curve The FadeCurve to convert to string
 * @return The corresponding string curve to the given FadeCurve 
 */
String Helper::FadeCurveToString(FadeCurve curve)
{

    switch (curve)
    {
    case FadeCurve::None:
        return "None";
        break;

    case FadeCurve::Linear:
        return "Linear";
        break;

    case FadeCurve::EaseIn:
        return "EaseIn";
        break;

    case FadeCurve::EaseOut:
        return "EaseOut";
        break;

    case FadeCurve::EaseInOut:
        return "EaseInOut";
        break;

    default:
        return "None";
        break;
    }
};

/**
 * @brief Prints a string message with length of insertLength * spaces to serial
 * 
 */
void Helper::InsertPrint()
{
    for (uint8_t i = 0; i < insertLength; i++)
    {
        Serial.print("");
    }
};

String Helper::LEDOutputTypeToString(LEDOutputType type)
{
    String msg = "";

    switch (type)
    {
    case LEDOutputType::R:
        msg = "R";
        break;
    case LEDOutputType::G:
        msg = "G";
        break;
    case LEDOutputType::B:
        msg = "B";
        break;
    case LEDOutputType::CW:
        msg = "CW";
        break;
    case LEDOutputType::WW:
        msg = "WW";
        break;
    }

    return msg;
}

uint8_t Helper::LEDOutputTypeToUint8(LEDOutputType type)
{
    uint8_t data = 0;

    switch (type)
    {
    case LEDOutputType::R:
        data = 0;
        break;
    case LEDOutputType::G:
        data = 1;
        break;
    case LEDOutputType::B:
        data = 2;
        break;
    case LEDOutputType::CW:
        data = 3;
        break;
    case LEDOutputType::WW:
        data = 4;
        break;
    }

    return data;
}

uint8_t Helper::MultiLEDEffectToUint8(MultiLEDEffect effect)
{
    uint8_t result = 0;

    switch (effect)
    {
    case MultiLEDEffect::Idle:
        result = 0;
        break;
    case MultiLEDEffect::SingleLEDEffect:
        result = 1;
        break;
    case MultiLEDEffect::MotionDetected:
        result = 2;
        break;
    case MultiLEDEffect::Alarm:
        result = 3;
        break;
    }

    return result;
}