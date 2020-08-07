#include "../include/Information.h"

Information::Information(Network *network, PirReader *pirReader)
{
    this->network = network;
    this->pirReader = pirReader;
};


/**
 * Does init stuff for the Information component
 * 
 * @return True if successfull, false if not 
 */
bool Information::Init()
{
    if (!init)
    {
        Serial.println("Information initialized");
        init = true;
    }

    return init;
};


/**
 * Runs the Information component. 
 */
void Information::Run()
{
    if(!init)
    {
        return;
    }

    // ---- Check for changes in Network parameter to memNetwork parameter
    // -- Sun
    if(this->network->parameter_sun != memNetwork.parameter_sun)
    {
        FormatPrintSingle(  "Sun", 
                            BoolToString(this->network->parameter_sun));  

        memNetwork.parameter_sun = this->network->parameter_sun;
    }

    // -- Time
    if( this->network->stNetworkTimeData.hour      != memNetwork.stNetworkTimeData.hour
        || this->network->stNetworkTimeData.minute != memNetwork.stNetworkTimeData.minute
        || this->network->stNetworkTimeData.second != memNetwork.stNetworkTimeData.second)
    {
        //FormatPrintTime("Time", 
        //                String(this->network->stNetworkTimeData.hour), 
        //                String(this->network->stNetworkTimeData.minute),
        //                String(this->network->stNetworkTimeData.second));   

        memNetwork.stNetworkTimeData.hour      = this->network->stNetworkTimeData.hour;
        memNetwork.stNetworkTimeData.minute    = this->network->stNetworkTimeData.minute;
        memNetwork.stNetworkTimeData.second    = this->network->stNetworkTimeData.second;
    }

    // -- Master
    if(this->network->parameter_master_present != memNetwork.parameter_master_present)
    {
        FormatPrintSingle(  "Master", 
                            BoolToString(this->network->parameter_master_present));  

        memNetwork.parameter_master_present = this->network->parameter_master_present;
    }

    // -- PC
    if(this->network->parameter_pc_present != memNetwork.parameter_pc_present)
    {
        FormatPrintSingle(  "PC", 
                            BoolToString(this->network->parameter_pc_present));  

        memNetwork.parameter_pc_present = this->network->parameter_pc_present;
    }

    
    // -- Motion Parameter
    if( this->network->stNetworkMotionData.power                != memNetwork.stNetworkMotionData.power
        || this->network->stNetworkMotionData.redColorValue     != memNetwork.stNetworkMotionData.redColorValue
        || this->network->stNetworkMotionData.greenColorValue   != memNetwork.stNetworkMotionData.greenColorValue
        || this->network->stNetworkMotionData.blueColorValue    != memNetwork.stNetworkMotionData.blueColorValue
        || this->network->stNetworkMotionData.timeout           != memNetwork.stNetworkMotionData.timeout)
    {
        FormatPrintMotion(  "Motion Parameter", 
                            BoolToString(this->network->stNetworkMotionData.power),
                            String(this->network->stNetworkMotionData.redColorValue),
                            String(this->network->stNetworkMotionData.greenColorValue),
                            String(this->network->stNetworkMotionData.blueColorValue),
                            String(this->network->stNetworkMotionData.timeout)); 

        memNetwork.stNetworkMotionData.power            = this->network->stNetworkMotionData.power;
        memNetwork.stNetworkMotionData.redColorValue    = this->network->stNetworkMotionData.redColorValue;
        memNetwork.stNetworkMotionData.greenColorValue  = this->network->stNetworkMotionData.greenColorValue;
        memNetwork.stNetworkMotionData.blueColorValue   = this->network->stNetworkMotionData.blueColorValue;
        memNetwork.stNetworkMotionData.timeout          = this->network->stNetworkMotionData.timeout;
    }

    
    // -- LED Strip 1
    // Only High Level LED Strip Data
    if( this->network->stNetworkLedStrip1Data.power                                 != memNetwork.stNetworkLedStrip1Data.power
        || this->network->stNetworkLedStrip1Data.ledStripData.brightnessValue       != memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessValue
        || this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeTime    != memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessFadeTime
        || this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve   != memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve
        || this->network->stNetworkLedStrip1Data.ledStripData.cwColorValue          != memNetwork.stNetworkLedStrip1Data.ledStripData.cwColorValue
        || this->network->stNetworkLedStrip1Data.ledStripData.wwColorValue          != memNetwork.stNetworkLedStrip1Data.ledStripData.wwColorValue
        || this->network->stNetworkLedStrip1Data.ledStripData.redColorValue         != memNetwork.stNetworkLedStrip1Data.ledStripData.redColorValue
        || this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue       != memNetwork.stNetworkLedStrip1Data.ledStripData.greenColorValue
        || this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue        != memNetwork.stNetworkLedStrip1Data.ledStripData.blueColorValue
        || this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime         != memNetwork.stNetworkLedStrip1Data.ledStripData.colorFadeTime
        || this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve        != memNetwork.stNetworkLedStrip1Data.ledStripData.colorFadeCurve
        || this->network->stNetworkLedStrip1Data.effect                             != memNetwork.stNetworkLedStrip1Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 1", 
                            BoolToString(this->network->stNetworkLedStrip1Data.power),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.brightnessValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.cwColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.wwColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.redColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve),
                            LEDEffectToString(this->network->stNetworkLedStrip1Data.effect)); 

        memNetwork.stNetworkLedStrip1Data.power                             = this->network->stNetworkLedStrip1Data.power;
        memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessValue      = this->network->stNetworkLedStrip1Data.ledStripData.brightnessValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessFadeTime   = this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeTime;
        memNetwork.stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve  = this->network->stNetworkLedStrip1Data.ledStripData.brightnessFadeCurve;
        memNetwork.stNetworkLedStrip1Data.ledStripData.cwColorValue         = this->network->stNetworkLedStrip1Data.ledStripData.cwColorValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.wwColorValue         = this->network->stNetworkLedStrip1Data.ledStripData.wwColorValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.redColorValue        = this->network->stNetworkLedStrip1Data.ledStripData.redColorValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.greenColorValue      = this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.blueColorValue       = this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue;
        memNetwork.stNetworkLedStrip1Data.ledStripData.colorFadeTime        = this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime;
        memNetwork.stNetworkLedStrip1Data.ledStripData.colorFadeCurve       = this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve;
        memNetwork.stNetworkLedStrip1Data.effect                            = this->network->stNetworkLedStrip1Data.effect;
    }

    // -- LED Strip 2
    // Only High Level LED Strip Data
    if( this->network->stNetworkLedStrip2Data.power                                 != memNetwork.stNetworkLedStrip2Data.power
        || this->network->stNetworkLedStrip2Data.ledStripData.brightnessValue       != memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessValue
        || this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeTime    != memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessFadeTime
        || this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve   != memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve
        || this->network->stNetworkLedStrip2Data.ledStripData.cwColorValue          != memNetwork.stNetworkLedStrip2Data.ledStripData.cwColorValue
        || this->network->stNetworkLedStrip2Data.ledStripData.wwColorValue          != memNetwork.stNetworkLedStrip2Data.ledStripData.wwColorValue
        || this->network->stNetworkLedStrip2Data.ledStripData.redColorValue         != memNetwork.stNetworkLedStrip2Data.ledStripData.redColorValue
        || this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue       != memNetwork.stNetworkLedStrip2Data.ledStripData.greenColorValue
        || this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue        != memNetwork.stNetworkLedStrip2Data.ledStripData.blueColorValue
        || this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime         != memNetwork.stNetworkLedStrip2Data.ledStripData.colorFadeTime
        || this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve        != memNetwork.stNetworkLedStrip2Data.ledStripData.colorFadeCurve
        || this->network->stNetworkLedStrip2Data.effect                             != memNetwork.stNetworkLedStrip2Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 2", 
                            BoolToString(this->network->stNetworkLedStrip2Data.power),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.brightnessValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.cwColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.wwColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.redColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve),
                            LEDEffectToString(this->network->stNetworkLedStrip2Data.effect)); 

        memNetwork.stNetworkLedStrip2Data.power                             = this->network->stNetworkLedStrip2Data.power;
        memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessValue      = this->network->stNetworkLedStrip2Data.ledStripData.brightnessValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessFadeTime   = this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeTime;
        memNetwork.stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve  = this->network->stNetworkLedStrip2Data.ledStripData.brightnessFadeCurve;
        memNetwork.stNetworkLedStrip2Data.ledStripData.cwColorValue         = this->network->stNetworkLedStrip2Data.ledStripData.cwColorValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.wwColorValue         = this->network->stNetworkLedStrip2Data.ledStripData.wwColorValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.redColorValue        = this->network->stNetworkLedStrip2Data.ledStripData.redColorValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.greenColorValue      = this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.blueColorValue       = this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue;
        memNetwork.stNetworkLedStrip2Data.ledStripData.colorFadeTime        = this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime;
        memNetwork.stNetworkLedStrip2Data.ledStripData.colorFadeCurve       = this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve;
        memNetwork.stNetworkLedStrip2Data.effect                            = this->network->stNetworkLedStrip2Data.effect;
    }
    

    // -- Motion Detection
    if( this->pirReader->motionDetected         != memPirReader.motionDetected
        || this->pirReader->sensor1Triggered    != memPirReader.sensor1Triggered
        || this->pirReader->sensor2Triggered    != memPirReader.sensor2Triggered)
    {
        FormatPrintMotionDetected(  BoolToString(this->pirReader->motionDetected),
                                    BoolToString(this->pirReader->sensor1Triggered),
                                    BoolToString(this->pirReader->sensor2Triggered));

        memPirReader.motionDetected     = this->pirReader->motionDetected ;
        memPirReader.sensor1Triggered   = this->pirReader->sensor1Triggered;
        memPirReader.sensor2Triggered   = this->pirReader->sensor2Triggered ;
    }

};


/**
 * Prints a single parameter/component formatted message to serial
 * 
 * @parameter name      The name of the parameter or component
 * @parameter value     The value of the given parameter or component
 **/
void Information::FormatPrintSingle(String name, 
                                    String value)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    // Parameter value
    InsertPrint();
    Serial.println("Value : " + value);

    BottomSpacerPrint();
};


/**
 * Prints a time formatted message to serial
 * 
 * @parameter name      The name of the parameter or component
 * @parameter hour      The value of the current hour
 * @parameter minute    The value of the current minute
 * @parameter second    The value of the current second
 **/
void Information::FormatPrintTime(  String name, 
                                    String hour, 
                                    String minute,
                                    String second)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    // Parameter value
    InsertPrint();
    Serial.println("Hour    : " + hour);
    Serial.println("Minute  : " + minute);
    Serial.println("Second  : " + second);

    BottomSpacerPrint();
};


/**
 * Prints a HighLevelLEDStripData formatted message to serial
 * 
 * @parameter name                  The name of the parameter or component
 * @parameter power                 The current power value of the LED strip
 * @parameter brightness            The current brightness value of the LED strip
 * @parameter brightnessFadeTime    The current brightness fade time of the LED strip
 * @parameter brightnessFadeCurve   The current brightness fade curve of the LED strip
 * @parameter cw                    The current cold white value of the LED strip
 * @parameter ww                    The current warm white value of the LED strip
 * @parameter red                   The current red value of the LED strip
 * @parameter green                 The current green value of the LED strip
 * @parameter blue                  The current blue value of the LED strip
 * @parameter colorFadeTime         The current color fade time of the LED strip
 * @parameter colorFadeCurve        The current color fade curve of the LED strip
 * @parameter effect                The current effect value of the LED strip
 **/
void Information::FormatPrintLEDStrip(String name,
                                      String power,
                                      String brightness,
                                      String brightnessFadeTime,
                                      String brightnessFadeCurve,
                                      String cw,
                                      String ww,
                                      String red,
                                      String green,
                                      String blue,
                                      String colorFadeTime,
                                      String colorFadeCurve,
                                      String effect)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    InsertPrint();
    Serial.println("Power                  : " + power);

    InsertPrint();
    Serial.println("Brightness Value       : " + brightness);

    InsertPrint();
    Serial.println("Brightness Fade Time   : " + brightnessFadeTime + " milliseconds");

    InsertPrint();
    Serial.println("Brightness Fade Curve  : " + brightnessFadeCurve);

    InsertPrint();
    Serial.println("Cold White Color Value : " + cw);

    InsertPrint();
    Serial.println("Warm White Color Value : " + ww);

    InsertPrint();
    Serial.println("Red Color Value        : " + red);

    InsertPrint();
    Serial.println("Green Color Value      : " + green);

    InsertPrint();
    Serial.println("Blue Color Value       : " + blue);

    InsertPrint();
    Serial.println("Color Fade Time        : " + colorFadeTime + " milliseconds");

    InsertPrint();
    Serial.println("Color Fade Curve       : " + colorFadeCurve);

    InsertPrint();
    Serial.println("Effect                 : " + effect);

    BottomSpacerPrint();
};


/**
 * Prints a motion formatted message to serial
 * 
 * @parameter name          The name of the parameter or component
 * @parameter motionPower   The current motion power value
 * @parameter red           The current motion color red value
 * @parameter green         The current motion color green value
 * @parameter blue          The current motion color blue value
 * @parameter timeout       The current motion timeout
 **/
void Information::FormatPrintMotion(    String name, 
                                        String motionPower, 
                                        String red,
                                        String green,
                                        String blue,
                                        String timeout)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    // Power
    InsertPrint();
    Serial.println("Power      : " + motionPower);

    // Red
    InsertPrint();
    Serial.println("Red        : " + red);

    // Green
    InsertPrint();
    Serial.println("Green      : " + green);

    // Blue
    InsertPrint();
    Serial.println("Blue       : " + blue);

    // Timeout
    InsertPrint();
    Serial.println("Timeout    : " + timeout);

    BottomSpacerPrint();
};


/**
 * Prints a motion Detected formatted message to serial
 * 
 * @parameter motionDetected    The current value of the motion
 * @parameter sensor1Triggered  The current value of the 1 pir motion sensor
 * @parameter sensor2Triggered  The current value of the 2 pir motion sensor
 **/
void Information::FormatPrintMotionDetected(String motionDetected,
                                            String sensor1Triggered,
                                            String sensor2Triggered)
{
    TopSpacerPrint();

    // Motion Detection change
    InsertPrint();
    Serial.println("! Motion Detection Change !");

    // Motion Detected
    InsertPrint();
    Serial.println("Motion Detected : " + motionDetected);

    // Sensor 1 Triggered
    InsertPrint();
    Serial.println("PIR Sensor 1    : " + sensor1Triggered);

    // Sensor 2 Triggered
    InsertPrint();
    Serial.println("PIR Sensor 2    : " + sensor2Triggered);

    BottomSpacerPrint();
};


/**
 * Prints a formatted top spacer message to serial
 **/
void Information::TopSpacerPrint()
{
    Serial.println("");
    Serial.print("# ");
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(" #");
    Serial.println("");
};


/**
 * Prints a formatted bottom spacer message to serial
 **/
void Information::BottomSpacerPrint()
{
    Serial.print("# ");
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(" #");
    Serial.println("");
};


/**
 * Converts a given bool value to the corresponding string message
 * 
 * @parameter b     The bool value to convert
 * 
 * @return The value of the bool as string
 **/
String Information::BoolToString(bool b)
{
    if(b)
    {
        return "true";
    }
    else
    {
        return "false";
    }
};


/**
 * Converts a LEDEffect to a String
 * 
 * @parameter effect    The LEDEffect to convert to string
 * 
 * @return effect The corresponding string effect to the given LEDEffect
 **/
String Information::LEDEffectToString(LEDEffect effect)
{
    switch (effect)
    {

        case LEDEffect::None:
            return "None";
            break;
    
        case LEDEffect::Alarm:
            return "Alarm";
            break;

        case LEDEffect::Music:
            return "Music";
            break;

        case LEDEffect::Sleep:
            return "Sleep";
            break;

        case LEDEffect::Weekend:
            return "Weekend";
            break;

        case LEDEffect::RGB:
            return "RGB";
            break;

        case LEDEffect::CW:
            return "CW";
            break;

        case LEDEffect::WW:
            return "WW";
            break;

        case LEDEffect::RGBCW:
            return "RGBCW";
            break;

        case LEDEffect::RGBWW:
            return "RGBWW";
            break;       

        case LEDEffect::CWWW:
            return "CWWW";
            break;      

        default:
            return "None";
            break;
    }
}


/**
 * Converts a FadeCurve to a String
 * 
 * @parameter curve    The FadeCurve to convert to string
 * 
 * @return curve    The corresponding string curve to the given FadeCurve
 **/
String Information::FadeCurveToString(FadeCurve curve)
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
 * Prints a string message with length of insertLength * spaces to serial
 **/
void Information::InsertPrint()
{
    for (uint8_t i = 0; i < insertLength; i++)
    {
        Serial.print("");
    }
};