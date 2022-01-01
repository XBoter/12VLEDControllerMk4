#include "Information.h"

/**
 * @brief Construct a new Information:: Information object
 * 
 */
Information::Information(){

};

/**
 * @brief Sets the needed refernce for the information class
 */
void Information::setReference(Helper *helper)
{
    this->helper = helper;
};

/**
 * @brief Initializes the information component
 * 
 * @return True if the initialization was successful
 */
bool Information::Init()
{
    if (!init)
    {
        Serial.println(F("Information initialized"));
        init = true;
    }

    return init;
};

/**
 * @brief Runs the information component
 * 
 */
void Information::Run()
{
    if (!init)
    {
        return;
    }
};

/**
 * @brief Prints a single parameter/component formatted message to serial
 * 
 * @param name      The name of the parameter or component
 * @param value     The value of the given parameter or component
 **/
void Information::FormatPrintSingle(String name,
                                    String value)
{
    this->helper->TopSpacerPrint();

    // Paramter name
    this->helper->InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Parameter value
    this->helper->InsertPrint();
    Serial.print(F("Value : "));
    Serial.println(value);

    this->helper->BottomSpacerPrint();
};

/**
 * @brief Prints a time formatted message to serial
 * 
 * @param name      The name of the parameter or component
 * @param hour      The value of the current hour
 * @param minute    The value of the current minute
 * @param second    The value of the current second
 **/
void Information::FormatPrintTime(String name,
                                  String hour,
                                  String minute,
                                  String second,
                                  String unix)
{
    this->helper->TopSpacerPrint();

    // Paramter name
    this->helper->InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Parameter value
    this->helper->InsertPrint();
    Serial.print(F("Hour    : "));
    Serial.println(hour);
    Serial.print(F("Minute  : "));
    Serial.println(minute);
    Serial.print(F("Second  : "));
    Serial.println(second);
    Serial.print(F("Unix    : "));
    Serial.println(unix);

    this->helper->BottomSpacerPrint();
};

/**
 * @brief Prints a HighLevelLEDStripData formatted message to serial
 * 
 * @param name                  The name of the parameter or component
 * @param power                 The current power value of the LED strip
 * @param brightness            The current brightness value of the LED strip
 * @param brightnessFadeTime    The current brightness fade time of the LED strip
 * @param brightnessFadeCurve   The current brightness fade curve of the LED strip
 * @param cw                    The current cold white value of the LED strip
 * @param ww                    The current warm white value of the LED strip
 * @param red                   The current red value of the LED strip
 * @param green                 The current green value of the LED strip
 * @param blue                  The current blue value of the LED strip
 * @param colorFadeTime         The current color fade time of the LED strip
 * @param colorFadeCurve        The current color fade curve of the LED strip
 * @param effect                The current effect value of the LED strip
 **/
void Information::FormatPrintLEDStrip(String name,
                                      String power,
                                      String red,
                                      String green,
                                      String blue,
                                      String colorFadeTime,
                                      String colorFadeCurve,
                                      String colorBrightness,
                                      String colorBrightnessFadeTime,
                                      String colorBrightnessFadeCurve,
                                      String whiteTemperature,
                                      String whiteTemperatureFadeTime,
                                      String whiteTemperatureFadeCurve,
                                      String whiteBrightness,
                                      String whiteBrightnessFadeTime,
                                      String whiteBrightnessFadeCurve,
                                      String effect)
{
    this->helper->TopSpacerPrint();

    // Paramter name
    this->helper->InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    this->helper->InsertPrint();
    Serial.print(F("Power                           : "));
    Serial.println(power);

    this->helper->InsertPrint();
    Serial.print(F("Red Color Value                 : "));
    Serial.println(red);

    this->helper->InsertPrint();
    Serial.print(F("Green Color Value               : "));
    Serial.println(green);

    this->helper->InsertPrint();
    Serial.print(F("Blue Color Value                : "));
    Serial.println(blue);

    this->helper->InsertPrint();
    Serial.print(F("Color Fade Time                 : "));
    Serial.print(colorFadeTime);
    Serial.println(F(" milliseconds"));

    this->helper->InsertPrint();
    Serial.print(F("Color Fade Curve                : "));
    Serial.println(colorFadeCurve);

    this->helper->InsertPrint();
    Serial.print(F("Color Brightness Value          : "));
    Serial.println(colorBrightness);

    this->helper->InsertPrint();
    Serial.print(F("Color Brightness Fade Time      : "));
    Serial.print(colorBrightnessFadeTime);
    Serial.println(F(" milliseconds"));

    this->helper->InsertPrint();
    Serial.print(F("Color Brightness Fade Curve     : "));
    Serial.println(colorBrightnessFadeCurve);

    this->helper->InsertPrint();
    Serial.print(F("White Temperature Value         : "));
    Serial.println(whiteTemperature);

    this->helper->InsertPrint();
    Serial.print(F("White Temperature Fade Time     : "));
    Serial.print(whiteTemperatureFadeTime);
    Serial.println(F(" milliseconds"));

    this->helper->InsertPrint();
    Serial.print(F("White Temperature Fade Curve    : "));
    Serial.println(whiteTemperatureFadeCurve);

    this->helper->InsertPrint();
    Serial.print(F("White Brightness Value          : "));
    Serial.println(whiteBrightness);

    this->helper->InsertPrint();
    Serial.print(F("White Brightness Fade Time      : "));
    Serial.print(whiteBrightnessFadeTime);
    Serial.println(F(" milliseconds"));

    this->helper->InsertPrint();
    Serial.print(F("White Brightness Fade Curve     : "));
    Serial.println(whiteBrightnessFadeCurve);

    this->helper->InsertPrint();
    Serial.print(F("Effect                          : "));
    Serial.println(effect);

    this->helper->BottomSpacerPrint();
};

/**
 * @brief Prints a motion formatted message to serial
 * 
 * @param name          The name of the parameter or component
 * @param motionPower   The current motion power value
 * @param red           The current motion color red value
 * @param green         The current motion color green value
 * @param blue          The current motion color blue value
 * @param timeout       The current motion timeout
 **/
void Information::FormatPrintMotion(String name,
                                    String motionDetectionEnabled,
                                    String timeBasedBrightnessChangeEnabled,
                                    String timeout,
                                    String red,
                                    String green,
                                    String blue,
                                    String colorBrightness,
                                    String whiteTemperature,
                                    String whiteBrightness)
{
    this->helper->TopSpacerPrint();

    // Paramter name
    this->helper->InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Motion Detection Enabled
    this->helper->InsertPrint();
    Serial.print(F("Motion Detection Enabled             : "));
    Serial.println(motionDetectionEnabled);

    // Time Based Brightness Change Enabled
    this->helper->InsertPrint();
    Serial.print(F("Time Based Brightness Change Enabled : "));
    Serial.println(timeBasedBrightnessChangeEnabled);

    // Timeout
    this->helper->InsertPrint();
    Serial.print(F("Timeout                              : "));
    Serial.println(timeout);

    // Red
    this->helper->InsertPrint();
    Serial.print(F("Red                                  : "));
    Serial.println(red);

    // Green
    this->helper->InsertPrint();
    Serial.print(F("Green                                : "));
    Serial.println(green);

    // Blue
    this->helper->InsertPrint();
    Serial.print(F("Blue                                 : "));
    Serial.println(blue);

    // Color Brightness
    this->helper->InsertPrint();
    Serial.print(F("Color Brightness                     : "));
    Serial.println(colorBrightness);

    // White Temperature
    this->helper->InsertPrint();
    Serial.print(F("White Temperature                    : "));
    Serial.println(whiteTemperature);

    // White Brightness
    this->helper->InsertPrint();
    Serial.print(F("White Brightness                     : "));
    Serial.println(whiteBrightness);

    this->helper->BottomSpacerPrint();
};

/**
 * @brief Prints a motion Detected formatted message to serial
 * 
 * @param motionDetected    The current value of the motion
 * @param sensor1Triggered  The current value of the 1 pir motion sensor
 * @param sensor2Triggered  The current value of the 2 pir motion sensor
 **/
void Information::FormatPrintMotionDetected(String motionDetected,
                                            String sensorTriggered,
                                            String sensor1Triggered,
                                            String sensor2Triggered,
                                            String virtualSensorTriggered)
{
    this->helper->TopSpacerPrint();

    // Motion Detection change
    this->helper->InsertPrint();
    Serial.println(F("! Motion Detection Change !"));

    // Motion Detected
    this->helper->InsertPrint();
    Serial.print(F("Motion Detected         : "));
    Serial.println(motionDetected);

    // Sensor 1 Triggered
    this->helper->InsertPrint();
    Serial.print(F("Combined PIR Sensor     : "));
    Serial.println(sensorTriggered);

    // Sensor 1 Triggered
    this->helper->InsertPrint();
    Serial.print(F("Physical PIR Sensor 1   : "));
    Serial.println(sensor1Triggered);

    // Sensor 2 Triggered
    this->helper->InsertPrint();
    Serial.print(F("Physical PIR Sensor 2   : "));
    Serial.println(sensor2Triggered);

    // Virtual Sensor Triggered
    this->helper->InsertPrint();
    Serial.print(F("Virtual PIR Sensor      : "));
    Serial.println(virtualSensorTriggered);

    this->helper->BottomSpacerPrint();
};

/**
 * Prints a network wifi formatted message to serial
 * 
 * @param connectionState   The current connection state of the WiFi connection
 * @param hostname          The current hostname of the client if connected
 * @param ipAddress         The current ip address of the esp if connected
 * @param subnetmask        The current subnetmask of the esp if connected
 * @param macAddress        The current macAddress of the esp
 * @param gateway           The current gateway of the esp if connected
 **/
void Information::FormatPrintNetworkWiFi(String connectionState,
                                         String hostname,
                                         String ipAddress,
                                         String subnetmask,
                                         String macAddress,
                                         String gateway)
{
    this->helper->TopSpacerPrint();

    // Network WiFi changed
    this->helper->InsertPrint();
    Serial.println(F("Network WiFi changed"));

    // Connection State
    this->helper->InsertPrint();
    Serial.print(F("Connection State        : "));
    Serial.println(connectionState);

    // Hostname
    this->helper->InsertPrint();
    Serial.print(F("Hostname                : "));
    Serial.println(hostname);

    // IP Address
    this->helper->InsertPrint();
    Serial.print(F("IP Address              : "));
    Serial.println(ipAddress);

    // Subnetmask
    this->helper->InsertPrint();
    Serial.print(F("Subnetmask              : "));
    Serial.println(subnetmask);

    // Mac Address
    this->helper->InsertPrint();
    Serial.print(F("Mac Address             : "));
    Serial.println(macAddress);

    // gateway
    this->helper->InsertPrint();
    Serial.print(F("Gateway                 : "));
    Serial.println(gateway);

    this->helper->BottomSpacerPrint();
};

/**
 * @brief Prints a network mqtt formatted message to serial
 * 
 * @param connectionState   The current connection state of the mqtt connection
 * @param clientStateInfo   The current client state info
 * @param brokerIpAddress   The broker ip address to connect to
 * @param clientName        The mqtt client name
 **/
void Information::FormatPrintNetworkMQTT(String connectionState,
                                         String clientStateInfo,
                                         String brokerIpAddress,
                                         int brokerPort,
                                         String clientName)
{
    this->helper->TopSpacerPrint();

    // Network MQTT changed
    this->helper->InsertPrint();
    Serial.println(F("Network MQTT changed"));

    // Connection State
    this->helper->InsertPrint();
    Serial.print(F("Connection State        : "));
    Serial.println(connectionState);

    // Client State Info
    this->helper->InsertPrint();
    Serial.print(F("Client State Info       : "));
    Serial.println(clientStateInfo);

    // Broker IP Address
    this->helper->InsertPrint();
    Serial.print(F("Broker IP Address       : "));
    Serial.println(brokerIpAddress);

    // Broker Port
    this->helper->InsertPrint();
    Serial.print(F("Broker Port             : "));
    Serial.println(brokerPort);

    // Client Name
    this->helper->InsertPrint();
    Serial.print(F("Client Name             : "));
    Serial.println(clientName);

    this->helper->BottomSpacerPrint();
};
