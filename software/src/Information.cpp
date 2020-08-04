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
    if( this->network->stTimeData.hour      != memNetwork.stTimeData.hour
        || this->network->stTimeData.minute != memNetwork.stTimeData.minute
        || this->network->stTimeData.second != memNetwork.stTimeData.second)
    {
        FormatPrintTime("Time", 
                        String(this->network->stTimeData.hour), 
                        String(this->network->stTimeData.minute),
                        String(this->network->stTimeData.second));   

        memNetwork.stTimeData.hour      = this->network->stTimeData.hour;
        memNetwork.stTimeData.minute    = this->network->stTimeData.minute;
        memNetwork.stTimeData.second    = this->network->stTimeData.second;
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
    if( this->network->stMotionData.power       != memNetwork.stMotionData.power
        || this->network->stMotionData.red      != memNetwork.stMotionData.red
        || this->network->stMotionData.green    != memNetwork.stMotionData.green
        || this->network->stMotionData.blue     != memNetwork.stMotionData.blue
        || this->network->stMotionData.timeout  != memNetwork.stMotionData.timeout)
    {
        FormatPrintMotion(  "Motion Parameter", 
                            BoolToString(this->network->stMotionData.power),
                            String(this->network->stMotionData.red),
                            String(this->network->stMotionData.green),
                            String(this->network->stMotionData.blue),
                            String(this->network->stMotionData.timeout)); 

        memNetwork.stMotionData.power   = this->network->stMotionData.power;
        memNetwork.stMotionData.red     = this->network->stMotionData.red;
        memNetwork.stMotionData.green   = this->network->stMotionData.green;
        memNetwork.stMotionData.blue    = this->network->stMotionData.blue;
        memNetwork.stMotionData.timeout = this->network->stMotionData.timeout;
    }

    
    // -- LED Strip 1
    if( this->network->stLedStrip1Data.power            != memNetwork.stLedStrip1Data.power
        || this->network->stLedStrip1Data.brightness    != memNetwork.stLedStrip1Data.brightness
        || this->network->stLedStrip1Data.cw            != memNetwork.stLedStrip1Data.cw
        || this->network->stLedStrip1Data.ww            != memNetwork.stLedStrip1Data.ww
        || this->network->stLedStrip1Data.red           != memNetwork.stLedStrip1Data.red
        || this->network->stLedStrip1Data.green         != memNetwork.stLedStrip1Data.green
        || this->network->stLedStrip1Data.blue          != memNetwork.stLedStrip1Data.blue
        || this->network->stLedStrip1Data.effect        != memNetwork.stLedStrip1Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 1", 
                            BoolToString(this->network->stLedStrip1Data.power),
                            String(this->network->stLedStrip1Data.brightness),
                            String(this->network->stLedStrip1Data.cw),
                            String(this->network->stLedStrip1Data.ww),
                            String(this->network->stLedStrip1Data.red),
                            String(this->network->stLedStrip1Data.green),
                            String(this->network->stLedStrip1Data.blue),
                            LEDEffectToString(this->network->stLedStrip1Data.effect)); 

        memNetwork.stLedStrip1Data.power        = this->network->stLedStrip1Data.power;
        memNetwork.stLedStrip1Data.brightness   = this->network->stLedStrip1Data.brightness;
        memNetwork.stLedStrip1Data.cw           = this->network->stLedStrip1Data.cw;
        memNetwork.stLedStrip1Data.ww           = this->network->stLedStrip1Data.ww;
        memNetwork.stLedStrip1Data.red          = this->network->stLedStrip1Data.red;
        memNetwork.stLedStrip1Data.green        = this->network->stLedStrip1Data.green;
        memNetwork.stLedStrip1Data.blue         = this->network->stLedStrip1Data.blue;
        memNetwork.stLedStrip1Data.effect       = this->network->stLedStrip1Data.effect;
    }

    // -- LED Strip 2
    if( this->network->stLedStrip2Data.power            != memNetwork.stLedStrip2Data.power
        || this->network->stLedStrip2Data.brightness    != memNetwork.stLedStrip2Data.brightness
        || this->network->stLedStrip2Data.cw            != memNetwork.stLedStrip2Data.cw
        || this->network->stLedStrip2Data.ww            != memNetwork.stLedStrip2Data.ww
        || this->network->stLedStrip2Data.red           != memNetwork.stLedStrip2Data.red
        || this->network->stLedStrip2Data.green         != memNetwork.stLedStrip2Data.green
        || this->network->stLedStrip2Data.blue          != memNetwork.stLedStrip2Data.blue
        || this->network->stLedStrip2Data.effect        != memNetwork.stLedStrip2Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 2", 
                            BoolToString(this->network->stLedStrip2Data.power),
                            String(this->network->stLedStrip2Data.brightness),
                            String(this->network->stLedStrip2Data.cw),
                            String(this->network->stLedStrip2Data.ww),
                            String(this->network->stLedStrip2Data.red),
                            String(this->network->stLedStrip2Data.green),
                            String(this->network->stLedStrip2Data.blue),
                            LEDEffectToString(this->network->stLedStrip2Data.effect)); 

        memNetwork.stLedStrip2Data.power        = this->network->stLedStrip2Data.power;
        memNetwork.stLedStrip2Data.brightness   = this->network->stLedStrip2Data.brightness;
        memNetwork.stLedStrip2Data.cw           = this->network->stLedStrip2Data.cw;
        memNetwork.stLedStrip2Data.ww           = this->network->stLedStrip2Data.ww;
        memNetwork.stLedStrip2Data.red          = this->network->stLedStrip2Data.red;
        memNetwork.stLedStrip2Data.green        = this->network->stLedStrip2Data.green;
        memNetwork.stLedStrip2Data.blue         = this->network->stLedStrip2Data.blue;
        memNetwork.stLedStrip2Data.effect       = this->network->stLedStrip2Data.effect;
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
 * Prints a led strip formatted message to serial
 * 
 * @parameter name          The name of the parameter or component
 * @parameter power         The current power value of the LED strip
 * @parameter brightness    The current brightness value of the LED strip
 * @parameter cw            The current cold white value of the LED strip
 * @parameter ww            The current warm white value of the LED strip
 * @parameter red           The current red value of the LED strip
 * @parameter green         The current green value of the LED strip
 * @parameter blue          The current blue value of the LED strip
 * @parameter effect        The current effect value of the LED strip
 **/
void Information::FormatPrintLEDStrip(  String name, 
                                        String power, 
                                        String brightness,
                                        String cw,
                                        String ww,
                                        String red,
                                        String green,
                                        String blue,
                                        String effect)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    // Power
    InsertPrint();
    Serial.println("Power      : " + power);

    // Brightness
    InsertPrint();
    Serial.println("Brightness : " + brightness);

    // Cold White
    InsertPrint();
    Serial.println("Cold White : " + cw);

    // Warm White
    InsertPrint();
    Serial.println("Warm White : " + ww);

    // Red
    InsertPrint();
    Serial.println("Red        : " + red);

    // Green
    InsertPrint();
    Serial.println("Green      : " + green);

    // Blue
    InsertPrint();
    Serial.println("Blue       : " + blue);

    // Effect
    InsertPrint();
    Serial.println("Effect     : " + effect);

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
        case LEDEffect::NoMasterPresent:
            return "NoMasterPresent";
            break;

        case LEDEffect::MotionDetected:
            return "MotionDetected";
            break;

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
 * Prints a string message with length of insertLength * spaces to serial
 **/
void Information::InsertPrint()
{
    for (uint8_t i = 0; i < insertLength; i++)
    {
        Serial.print("");
    }
};