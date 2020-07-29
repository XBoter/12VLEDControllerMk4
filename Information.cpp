#include "Information.h"

using namespace LedControllerSoftwareMk5;

Information::Information(Network *network, PirReader *pirReader)
{
    this->network = network;
    this->pirReader = pirReader;
};


/**
 * Initializes the information instance
 * @parameter None
 * @return None
 **/
void Information::Init()
{
    if (!init)
    {
        Serial.println("Information initialized");
        init = true;
    }
};


/**
 * Needs to get called every cycle. 
 * Prints changes of the defined paramters in the network class 
 * @parameter None
 * @return None
 **/
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
    if( this->network->parameter_time_hour      != memNetwork.parameter_time_hour
        || this->network->parameter_time_minute != memNetwork.parameter_time_minute)
    {
        FormatPrintTime("Time", 
                        String(this->network->parameter_time_hour), 
                        String(this->network->parameter_time_minute));   

        memNetwork.parameter_time_hour      = this->network->parameter_time_hour;
        memNetwork.parameter_time_minute    = this->network->parameter_time_minute;
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
    if( this->network->paramter_motion_detection_power  != memNetwork.paramter_motion_detection_power
        || this->network->parameter_motion_red_value    != memNetwork.parameter_motion_red_value
        || this->network->parameter_motion_green_value  != memNetwork.parameter_motion_green_value
        || this->network->parameter_motion_blue_value   != memNetwork.parameter_motion_blue_value
        || this->network->parameter_motion_timeout      != memNetwork.parameter_motion_timeout)
    {
        FormatPrintMotion(  "Motion Parameter", 
                            BoolToString(this->network->paramter_motion_detection_power),
                            String(this->network->parameter_motion_red_value ),
                            String(this->network->parameter_motion_green_value),
                            String(this->network->parameter_motion_blue_value),
                            String(this->network->parameter_motion_timeout)); 

        memNetwork.paramter_motion_detection_power  = this->network->paramter_motion_detection_power;
        memNetwork.parameter_motion_red_value       = this->network->parameter_motion_red_value;
        memNetwork.parameter_motion_green_value     = this->network->parameter_motion_green_value;
        memNetwork.parameter_motion_blue_value      = this->network->parameter_motion_blue_value;
        memNetwork.parameter_motion_timeout         = this->network->parameter_motion_timeout;
    }

    // -- LED Strip 1
    if( this->network->parameter_led_strip_1_power                  != memNetwork.parameter_led_strip_1_power
        || this->network->parameter_led_strip_1_brightness          != memNetwork.parameter_led_strip_1_brightness
        || this->network->parameter_led_strip_1_cold_white_value    != memNetwork.parameter_led_strip_1_cold_white_value
        || this->network->parameter_led_strip_1_warm_white_value    != memNetwork.parameter_led_strip_1_warm_white_value
        || this->network->parameter_led_strip_1_red_value           != memNetwork.parameter_led_strip_1_red_value
        || this->network->parameter_led_strip_1_green_value         != memNetwork.parameter_led_strip_1_green_value
        || this->network->parameter_led_strip_1_blue_value          != memNetwork.parameter_led_strip_1_blue_value
        || this->network->parameter_led_strip_1_effect              != memNetwork.parameter_led_strip_1_effect)
    {
        FormatPrintLEDStrip("LED Strip 1", 
                            BoolToString(this->network->parameter_led_strip_1_power),
                            String(this->network->parameter_led_strip_1_brightness),
                            String(this->network->parameter_led_strip_1_cold_white_value),
                            String(this->network->parameter_led_strip_1_warm_white_value),
                            String(this->network->parameter_led_strip_1_red_value),
                            String(this->network->parameter_led_strip_1_green_value),
                            String(this->network->parameter_led_strip_1_blue_value),
                            EffectToString(this->network->parameter_led_strip_1_effect)); 

        memNetwork.parameter_led_strip_1_power              = this->network->parameter_led_strip_1_power;
        memNetwork.parameter_led_strip_1_brightness         = this->network->parameter_led_strip_1_brightness;
        memNetwork.parameter_led_strip_1_cold_white_value   = this->network->parameter_led_strip_1_cold_white_value;
        memNetwork.parameter_led_strip_1_warm_white_value   = this->network->parameter_led_strip_1_warm_white_value;
        memNetwork.parameter_led_strip_1_red_value          = this->network->parameter_led_strip_1_red_value;
        memNetwork.parameter_led_strip_1_green_value        = this->network->parameter_led_strip_1_green_value;
        memNetwork.parameter_led_strip_1_blue_value         = this->network->parameter_led_strip_1_blue_value;
        memNetwork.parameter_led_strip_1_effect             = this->network->parameter_led_strip_1_effect;
    }

    // -- LED Strip 2
    if( this->network->parameter_led_strip_2_power                  != memNetwork.parameter_led_strip_2_power
        || this->network->parameter_led_strip_2_brightness          != memNetwork.parameter_led_strip_2_brightness
        || this->network->parameter_led_strip_2_cold_white_value    != memNetwork.parameter_led_strip_2_cold_white_value
        || this->network->parameter_led_strip_2_warm_white_value    != memNetwork.parameter_led_strip_2_warm_white_value
        || this->network->parameter_led_strip_2_red_value           != memNetwork.parameter_led_strip_2_red_value
        || this->network->parameter_led_strip_2_green_value         != memNetwork.parameter_led_strip_2_green_value
        || this->network->parameter_led_strip_2_blue_value          != memNetwork.parameter_led_strip_2_blue_value
        || this->network->parameter_led_strip_2_effect              != memNetwork.parameter_led_strip_2_effect)
    {
        FormatPrintLEDStrip("LED Strip 2", 
                            BoolToString(this->network->parameter_led_strip_2_power),
                            String(this->network->parameter_led_strip_2_brightness),
                            String(this->network->parameter_led_strip_2_cold_white_value),
                            String(this->network->parameter_led_strip_2_warm_white_value),
                            String(this->network->parameter_led_strip_2_red_value),
                            String(this->network->parameter_led_strip_2_green_value),
                            String(this->network->parameter_led_strip_2_blue_value),
                            EffectToString(this->network->parameter_led_strip_2_effect)); 

        memNetwork.parameter_led_strip_2_power              = this->network->parameter_led_strip_2_power;
        memNetwork.parameter_led_strip_2_brightness         = this->network->parameter_led_strip_2_brightness;
        memNetwork.parameter_led_strip_2_cold_white_value   = this->network->parameter_led_strip_2_cold_white_value;
        memNetwork.parameter_led_strip_2_warm_white_value   = this->network->parameter_led_strip_2_warm_white_value;
        memNetwork.parameter_led_strip_2_red_value          = this->network->parameter_led_strip_2_red_value;
        memNetwork.parameter_led_strip_2_green_value        = this->network->parameter_led_strip_2_green_value;
        memNetwork.parameter_led_strip_2_blue_value         = this->network->parameter_led_strip_2_blue_value;
        memNetwork.parameter_led_strip_2_effect             = this->network->parameter_led_strip_2_effect;
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
 * Prints a single formatted paramter to serial
 * @parameter The name of the parameter and the corresponding value of that parameter
 * @return None
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
 * Prints a time formatted paramter to serial
 * @parameter The name of the parameter and the corresponding value of that parameter
 * @return None
 **/
void Information::FormatPrintTime(  String name, 
                                    String hour, 
                                    String minute)
{   
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.println("Paramter/Component '" + name + "' changed");
    
    // Parameter value
    InsertPrint();
    Serial.println("Value : " + hour + ":" + minute);

    BottomSpacerPrint();
};


/**
 * Prints a led strip formatted paramter to serial
 * @parameter The name of the parameter and the corresponding value of that parameter
 * @return None
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
 * Prints a motion formatted paramter to serial
 * @parameter The name of the parameter and the corresponding value of that parameter
 * @return None
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
 * Prints a motion detected formatted paramter to serial
 * @parameter motionDetected and sensor values
 * @return None
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
 * Prints a formatted top spacer to serial
 * @parameter None
 * @return None
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
 * Prints a formatted bottom spacer to serial
 * @parameter None
 * @return None
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
 * Returns a string representing the state of the given bool
 * @parameter Bool value
 * @return String name of bool value state
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

String Information::EffectToString(LEDEffect effect)
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
 * Prints a string with length of insertLength * spaces
 * @parameter None
 * @return None
 **/
void Information::InsertPrint()
{
    for (uint8_t i = 0; i < insertLength; i++)
    {
        Serial.print("");
    }
};