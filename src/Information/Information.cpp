#include "Information.h"

Information::Information(){

};

/**
 * Sets reference to external components
 */
void Information::setReference(Network *network,
                               Network *memNetwork,
                               PirReader *pirReader,
                               PirReader *memPirReader)
{
    this->network = network;
    this->memNetwork = memNetwork;
    this->pirReader = pirReader;
    this->memPirReader = memPirReader;
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
        Serial.println(F("Information initialized"));
        init = true;
    }

    return init;
};

/**
 * Runs the Information component. 
 */
void Information::Run()
{
    if (!init)
    {
        return;
    }

    // ---- Check for changes in Network parameter to memNetwork parameter
    // -- Sun
    if (this->network->parameter_sun != memNetwork->parameter_sun)
    {
        FormatPrintSingle("Sun",
                          BoolToString(this->network->parameter_sun));

        memNetwork->parameter_sun = this->network->parameter_sun;
    }

    // -- Time
    if (this->network->stNetworkTimeData.hour != memNetwork->stNetworkTimeData.hour ||
        this->network->stNetworkTimeData.minute != memNetwork->stNetworkTimeData.minute ||
        this->network->stNetworkTimeData.second != memNetwork->stNetworkTimeData.second ||
        this->network->stNetworkTimeData.unix != memNetwork->stNetworkTimeData.unix)
    {
        FormatPrintTime("Time",
                        String(this->network->stNetworkTimeData.hour),
                        String(this->network->stNetworkTimeData.minute),
                        String(this->network->stNetworkTimeData.second),
                        String(this->network->stNetworkTimeData.unix));

        memNetwork->stNetworkTimeData.hour = this->network->stNetworkTimeData.hour;
        memNetwork->stNetworkTimeData.minute = this->network->stNetworkTimeData.minute;
        memNetwork->stNetworkTimeData.second = this->network->stNetworkTimeData.second;
        memNetwork->stNetworkTimeData.unix = this->network->stNetworkTimeData.unix;
    }

    // -- Master
    if (this->network->parameter_master_present != memNetwork->parameter_master_present)
    {
        FormatPrintSingle("Master",
                          BoolToString(this->network->parameter_master_present));

        memNetwork->parameter_master_present = this->network->parameter_master_present;
    }

    // -- Network WiFi
    if (this->network->wifiConnected != memNetwork->wifiConnected ||
        this->network->hostname != memNetwork->hostname ||
        this->network->ipAddress != memNetwork->ipAddress ||
        this->network->subnetmask != memNetwork->subnetmask ||
        this->network->macAddress != memNetwork->macAddress ||
        this->network->gateway != memNetwork->gateway )
    {  
        FormatPrintNetworkWiFi(BollToConnectionState(this->network->wifiConnected),
                               this->network->hostname,
                               this->network->ipAddress,
                               this->network->subnetmask,
                               this->network->macAddress,
                               this->network->gateway);

        memNetwork->wifiConnected = this->network->wifiConnected;
        memNetwork->hostname = this->network->hostname;
        memNetwork->ipAddress = this->network->ipAddress;
        memNetwork->subnetmask = this->network->subnetmask;
        memNetwork->macAddress = this->network->macAddress;
        memNetwork->gateway = this->network->gateway;
    }

    // -- Network MQTT
    if (this->network->mqttConnected != memNetwork->mqttConnected ||
        this->network->clientState != memNetwork->clientState ||
        this->network->brokerIpAddress != memNetwork->brokerIpAddress ||
        this->network->brokerPort != memNetwork->brokerPort ||
        this->network->clientName != memNetwork->clientName)
    {
        FormatPrintNetworkMQTT(BollToConnectionState(this->network->mqttConnected),
                               IntToClientStateInfo(this->network->clientState),
                               this->network->brokerIpAddress,
                               this->network->brokerPort,
                               this->network->clientName);

        memNetwork->mqttConnected = this->network->mqttConnected;
        memNetwork->clientState = this->network->clientState;
        memNetwork->brokerIpAddress = this->network->brokerIpAddress;
        memNetwork->brokerPort = this->network->brokerPort;
        memNetwork->clientName = this->network->clientName;
    }

    // -- Motion Parameter
    if (this->network->stNetworkMotionData.motionDetectionEnabled != memNetwork->stNetworkMotionData.motionDetectionEnabled ||
        this->network->stNetworkMotionData.timeBasedBrightnessChangeEnabled != memNetwork->stNetworkMotionData.timeBasedBrightnessChangeEnabled ||
        this->network->stNetworkMotionData.timeout != memNetwork->stNetworkMotionData.timeout ||
        this->network->stNetworkMotionData.redColorValue != memNetwork->stNetworkMotionData.redColorValue ||
        this->network->stNetworkMotionData.greenColorValue != memNetwork->stNetworkMotionData.greenColorValue ||
        this->network->stNetworkMotionData.blueColorValue != memNetwork->stNetworkMotionData.blueColorValue ||
        this->network->stNetworkMotionData.colorBrightnessValue != memNetwork->stNetworkMotionData.colorBrightnessValue ||
        this->network->stNetworkMotionData.whiteTemperatureValue != memNetwork->stNetworkMotionData.whiteTemperatureValue ||
        this->network->stNetworkMotionData.whiteBrightnessValue != memNetwork->stNetworkMotionData.whiteBrightnessValue)
    {
        FormatPrintMotion("Motion Parameter",
                          BoolToString(this->network->stNetworkMotionData.motionDetectionEnabled),
                          BoolToString(this->network->stNetworkMotionData.timeBasedBrightnessChangeEnabled),
                          String(this->network->stNetworkMotionData.timeout),
                          String(this->network->stNetworkMotionData.redColorValue),
                          String(this->network->stNetworkMotionData.greenColorValue),
                          String(this->network->stNetworkMotionData.blueColorValue),
                          String(this->network->stNetworkMotionData.colorBrightnessValue),
                          String(this->network->stNetworkMotionData.whiteTemperatureValue),
                          String(this->network->stNetworkMotionData.whiteBrightnessValue));

        memNetwork->stNetworkMotionData.motionDetectionEnabled = this->network->stNetworkMotionData.motionDetectionEnabled;
        memNetwork->stNetworkMotionData.timeBasedBrightnessChangeEnabled = this->network->stNetworkMotionData.timeBasedBrightnessChangeEnabled;
        memNetwork->stNetworkMotionData.timeout = this->network->stNetworkMotionData.timeout;
        memNetwork->stNetworkMotionData.redColorValue = this->network->stNetworkMotionData.redColorValue;
        memNetwork->stNetworkMotionData.greenColorValue = this->network->stNetworkMotionData.greenColorValue;
        memNetwork->stNetworkMotionData.blueColorValue = this->network->stNetworkMotionData.blueColorValue;
        memNetwork->stNetworkMotionData.colorBrightnessValue = this->network->stNetworkMotionData.colorBrightnessValue;
        memNetwork->stNetworkMotionData.whiteTemperatureValue = this->network->stNetworkMotionData.whiteTemperatureValue;
        memNetwork->stNetworkMotionData.whiteBrightnessValue = this->network->stNetworkMotionData.whiteBrightnessValue;
    }

    // -- LED Strip 1
    // Only High Level LED Strip Data
    if (this->network->stNetworkLedStrip1Data.power != memNetwork->stNetworkLedStrip1Data.power ||
        this->network->stNetworkLedStrip1Data.ledStripData.redColorValue != memNetwork->stNetworkLedStrip1Data.ledStripData.redColorValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue != memNetwork->stNetworkLedStrip1Data.ledStripData.greenColorValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue != memNetwork->stNetworkLedStrip1Data.ledStripData.blueColorValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime != memNetwork->stNetworkLedStrip1Data.ledStripData.colorFadeTime ||
        this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve != memNetwork->stNetworkLedStrip1Data.ledStripData.colorFadeCurve ||
        this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessValue != memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime != memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime ||
        this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve != memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime ||
        this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve != memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve ||
        this->network->stNetworkLedStrip1Data.effect != memNetwork->stNetworkLedStrip1Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 1",
                            BoolToString(this->network->stNetworkLedStrip1Data.power),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.redColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue),
                            String(this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve),
                            SingleLEDEffectToString(this->network->stNetworkLedStrip1Data.effect));

        memNetwork->stNetworkLedStrip1Data.power = this->network->stNetworkLedStrip1Data.power;
        memNetwork->stNetworkLedStrip1Data.ledStripData.redColorValue = this->network->stNetworkLedStrip1Data.ledStripData.redColorValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.greenColorValue = this->network->stNetworkLedStrip1Data.ledStripData.greenColorValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.blueColorValue = this->network->stNetworkLedStrip1Data.ledStripData.blueColorValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.colorFadeTime = this->network->stNetworkLedStrip1Data.ledStripData.colorFadeTime;
        memNetwork->stNetworkLedStrip1Data.ledStripData.colorFadeCurve = this->network->stNetworkLedStrip1Data.ledStripData.colorFadeCurve;
        memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessValue = this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime = this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeTime;
        memNetwork->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve = this->network->stNetworkLedStrip1Data.ledStripData.colorBrightnessFadeCurve;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue = this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime = this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeTime;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve = this->network->stNetworkLedStrip1Data.ledStripData.whiteTemperatureFadeCurve;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue = this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime = this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeTime;
        memNetwork->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve = this->network->stNetworkLedStrip1Data.ledStripData.whiteBrightnessFadeCurve;
        memNetwork->stNetworkLedStrip1Data.effect = this->network->stNetworkLedStrip1Data.effect;
    }

    // -- LED Strip 2
    // Only High Level LED Strip Data
    if (this->network->stNetworkLedStrip2Data.power != memNetwork->stNetworkLedStrip2Data.power ||
        this->network->stNetworkLedStrip2Data.ledStripData.redColorValue != memNetwork->stNetworkLedStrip2Data.ledStripData.redColorValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue != memNetwork->stNetworkLedStrip2Data.ledStripData.greenColorValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue != memNetwork->stNetworkLedStrip2Data.ledStripData.blueColorValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime != memNetwork->stNetworkLedStrip2Data.ledStripData.colorFadeTime ||
        this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve != memNetwork->stNetworkLedStrip2Data.ledStripData.colorFadeCurve ||
        this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessValue != memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime != memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime ||
        this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve != memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime ||
        this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve != memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve ||
        this->network->stNetworkLedStrip2Data.effect != memNetwork->stNetworkLedStrip2Data.effect)
    {
        FormatPrintLEDStrip("LED Strip 2",
                            BoolToString(this->network->stNetworkLedStrip2Data.power),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.redColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue),
                            String(this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime),
                            FadeCurveToString(this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve),
                            SingleLEDEffectToString(this->network->stNetworkLedStrip2Data.effect));

        memNetwork->stNetworkLedStrip2Data.power = this->network->stNetworkLedStrip2Data.power;
        memNetwork->stNetworkLedStrip2Data.ledStripData.redColorValue = this->network->stNetworkLedStrip2Data.ledStripData.redColorValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.greenColorValue = this->network->stNetworkLedStrip2Data.ledStripData.greenColorValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.blueColorValue = this->network->stNetworkLedStrip2Data.ledStripData.blueColorValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.colorFadeTime = this->network->stNetworkLedStrip2Data.ledStripData.colorFadeTime;
        memNetwork->stNetworkLedStrip2Data.ledStripData.colorFadeCurve = this->network->stNetworkLedStrip2Data.ledStripData.colorFadeCurve;
        memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessValue = this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime = this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeTime;
        memNetwork->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve = this->network->stNetworkLedStrip2Data.ledStripData.colorBrightnessFadeCurve;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue = this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime = this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeTime;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve = this->network->stNetworkLedStrip2Data.ledStripData.whiteTemperatureFadeCurve;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue = this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime = this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeTime;
        memNetwork->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve = this->network->stNetworkLedStrip2Data.ledStripData.whiteBrightnessFadeCurve;
        memNetwork->stNetworkLedStrip2Data.effect = this->network->stNetworkLedStrip2Data.effect;
    }

    // -- Motion Detection
    if (this->pirReader->motionDetected != memPirReader->motionDetected ||
        this->pirReader->sensor1Triggered != memPirReader->sensor1Triggered ||
        this->pirReader->sensor2Triggered != memPirReader->sensor2Triggered ||
        this->pirReader->virtualSensorTriggered != memPirReader->virtualSensorTriggered)
    {
        FormatPrintMotionDetected(BoolToString(this->pirReader->motionDetected),
                                  BoolToString(this->pirReader->sensorTriggered),
                                  BoolToString(this->pirReader->sensor1Triggered),
                                  BoolToString(this->pirReader->sensor2Triggered),
                                  BoolToString(this->pirReader->virtualSensorTriggered));

        this->network->PublishMotionDetected();

        memPirReader->motionDetected = this->pirReader->motionDetected;
        memPirReader->sensor1Triggered = this->pirReader->sensor1Triggered;
        memPirReader->sensor2Triggered = this->pirReader->sensor2Triggered;
        memPirReader->virtualSensorTriggered = this->pirReader->virtualSensorTriggered;
    }
};

/**
 * Prints a single parameter/component formatted message to serial
 * 
 * @param name      The name of the parameter or component
 * @param value     The value of the given parameter or component
 **/
void Information::FormatPrintSingle(String name,
                                    String value)
{
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Parameter value
    InsertPrint();
    Serial.print(F("Value : "));
    Serial.println(value);

    BottomSpacerPrint();
};

/**
 * Prints a time formatted message to serial
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
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Parameter value
    InsertPrint();
    Serial.print(F("Hour    : "));
    Serial.println(hour);
    Serial.print(F("Minute  : "));
    Serial.println(minute);
    Serial.print(F("Second  : "));
    Serial.println(second);
    Serial.print(F("Unix    : "));
    Serial.println(unix);

    BottomSpacerPrint();
};

/**
 * Prints a HighLevelLEDStripData formatted message to serial
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
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    InsertPrint();
    Serial.print(F("Power                           : "));
    Serial.println(power);

    InsertPrint();
    Serial.print(F("Red Color Value                 : "));
    Serial.println(red);

    InsertPrint();
    Serial.print(F("Green Color Value               : "));
    Serial.println(green);

    InsertPrint();
    Serial.print(F("Blue Color Value                : "));
    Serial.println(blue);

    InsertPrint();
    Serial.print(F("Color Fade Time                 : "));
    Serial.print(colorFadeTime);
    Serial.println(F("milliseconds"));

    InsertPrint();
    Serial.print(F("Color Fade Curve                : "));
    Serial.println(colorFadeCurve);

    InsertPrint();
    Serial.print(F("Color Brightness Value          : "));
    Serial.println(colorBrightness);

    InsertPrint();
    Serial.print(F("Color Brightness Fade Time      : "));
    Serial.print(colorBrightnessFadeTime);
    Serial.println(F("milliseconds"));

    InsertPrint();
    Serial.print(F("Color Brightness Fade Curve     : "));
    Serial.println(colorBrightnessFadeCurve);

    InsertPrint();
    Serial.print(F("White Temperature Value         : "));
    Serial.println(whiteTemperature);

    InsertPrint();
    Serial.print(F("White Temperature Fade Time     : "));
    Serial.print(whiteTemperatureFadeTime);
    Serial.println(F("milliseconds"));

    InsertPrint();
    Serial.print(F("White Temperature Fade Curve    : "));
    Serial.println(whiteTemperatureFadeCurve);

    InsertPrint();
    Serial.print(F("White Brightness Value          : "));
    Serial.println(whiteBrightness);

    InsertPrint();
    Serial.print(F("White Brightness Fade Time      : "));
    Serial.print(whiteBrightnessFadeTime);
    Serial.println(F("milliseconds"));

    InsertPrint();
    Serial.print(F("White Brightness Fade Curve     : "));
    Serial.println(whiteBrightnessFadeCurve);

    InsertPrint();
    Serial.print(F("Effect                          : "));
    Serial.println(effect);

    BottomSpacerPrint();
};

/**
 * Prints a motion formatted message to serial
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
    TopSpacerPrint();

    // Paramter name
    InsertPrint();
    Serial.print(F("Paramter/Component/Mode '"));
    Serial.print(name);
    Serial.println(F("' changed"));

    // Motion Detection Enabled
    InsertPrint();
    Serial.print(F("Motion Detection Enabled             : "));
    Serial.println(motionDetectionEnabled);

    // Time Based Brightness Change Enabled
    InsertPrint();
    Serial.print(F("Time Based Brightness Change Enabled : "));
    Serial.println(timeBasedBrightnessChangeEnabled);

    // Timeout
    InsertPrint();
    Serial.print(F("Timeout                              : "));
    Serial.println(timeout);

    // Red
    InsertPrint();
    Serial.print(F("Red                                  : "));
    Serial.println(red);

    // Green
    InsertPrint();
    Serial.print(F("Green                                : "));
    Serial.println(green);

    // Blue
    InsertPrint();
    Serial.print(F("Blue                                 : "));
    Serial.println(blue);

    // Color Brightness
    InsertPrint();
    Serial.print(F("Color Brightness                     : "));
    Serial.println(colorBrightness);

    // White Temperature
    InsertPrint();
    Serial.print(F("White Temperature                    : "));
    Serial.println(whiteTemperature);

    // White Brightness
    InsertPrint();
    Serial.print(F("White Brightness                     : "));
    Serial.println(whiteBrightness);

    BottomSpacerPrint();
};

/**
 * Prints a motion Detected formatted message to serial
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
    TopSpacerPrint();

    // Motion Detection change
    InsertPrint();
    Serial.println(F("! Motion Detection Change !"));

    // Motion Detected
    InsertPrint();
    Serial.print(F("Motion Detected         : "));
    Serial.println(motionDetected);

    // Sensor 1 Triggered
    InsertPrint();
    Serial.print(F("Combined PIR Sensor     : "));
    Serial.println(sensorTriggered);

    // Sensor 1 Triggered
    InsertPrint();
    Serial.print(F("Physical PIR Sensor 1   : "));
    Serial.println(sensor1Triggered);

    // Sensor 2 Triggered
    InsertPrint();
    Serial.print(F("Physical PIR Sensor 2   : "));
    Serial.println(sensor2Triggered);

    // Virtual Sensor Triggered
    InsertPrint();
    Serial.print(F("Virtual PIR Sensor      : "));
    Serial.println(virtualSensorTriggered);

    BottomSpacerPrint();
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
    TopSpacerPrint();

    // Network WiFi changed
    InsertPrint();
    Serial.println(F("Network WiFi changed"));

    // Connection State
    InsertPrint();
    Serial.print(F("Connection State        : "));
    Serial.println(connectionState);

    // Hostname
    InsertPrint();
    Serial.print(F("Hostname                : "));
    Serial.println(hostname);

    // IP Address
    InsertPrint();
    Serial.print(F("IP Address              : "));
    Serial.println(ipAddress);

    // Subnetmask
    InsertPrint();
    Serial.print(F("Subnetmask              : "));
    Serial.println(subnetmask);

    // Mac Address
    InsertPrint();
    Serial.print(F("Mac Address             : "));
    Serial.println(macAddress);

    // gateway
    InsertPrint();
    Serial.print(F("Gateway                 : "));
    Serial.println(gateway);

    BottomSpacerPrint();
};


/**
 * Prints a network mqtt formatted message to serial
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
    TopSpacerPrint();

    // Network MQTT changed
    InsertPrint();
    Serial.println(F("Network MQTT changed"));

    // Connection State
    InsertPrint();
    Serial.print(F("Connection State        : "));
    Serial.println(connectionState);

    // Client State Info
    InsertPrint();
    Serial.print(F("Client State Info       : "));
    Serial.println(clientStateInfo);

    // Broker IP Address
    InsertPrint();
    Serial.print(F("Broker IP Address       : "));
    Serial.println(brokerIpAddress);

    // Broker Port
    InsertPrint();
    Serial.print(F("Broker Port             : "));
    Serial.println(brokerPort);

    // Client Name
    InsertPrint();
    Serial.print(F("Client Name             : "));
    Serial.println(clientName);

    BottomSpacerPrint();
};


/**
 * Prints a formatted top spacer message to serial
 **/
void Information::TopSpacerPrint()
{
    Serial.println(F(""));
    Serial.print(" # ");
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(" #");
    Serial.println(F(""));
};

/**
 * Prints a formatted bottom spacer message to serial
 **/
void Information::BottomSpacerPrint()
{
    Serial.print(" # ");
    for (uint8_t i = 0; i < spacerLength; i++)
    {
        Serial.print(symbol);
    }
    Serial.print(" #");
    Serial.println(F(""));
};

/**
 * Converts a given bool value to the corresponding string message
 * 
 * @param b     The bool value to convert
 * 
 * @return The value of the bool as string
 **/
String Information::BoolToString(bool b)
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
 * Converts a given bool value to the corresponding connection state
 * 
 * @param b     The bool value to convert
 * 
 * @return The value of the bool as connection state
 **/
String Information::BollToConnectionState(bool b)
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
 * Converts a given int value to the corresponding client state info
 * 
 * @param i     The int value to convert
 * 
 * @return The value of the int as client state info
 **/
String Information::IntToClientStateInfo(int i)
{
    switch(i)
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
 * Converts a SingleLEDEffect to a String
 * 
 * @param effect    The SingleLEDEffect to convert to string
 * 
 * @return effect The corresponding string effect to the given SingleLEDEffect
 **/
String Information::SingleLEDEffectToString(SingleLEDEffect effect)
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
 * Converts a string to a LEDEffect
 * 
 * @parameter effect    The name of effect as string
 * 
 * @return effect The corresponding LEDEffect to the given string effect
 **/
SingleLEDEffect Information::StringToSingleLEDEffect(String effect)
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
 * Converts a MultiLEDEffect to a String
 * 
 * @param effect    The MultiLEDEffect to convert to string
 * 
 * @return effect The corresponding string effect to the given MultiLEDEffect
 **/
String Information::MultiLEDEffectToString(MultiLEDEffect effect)
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
 * Converts a FadeCurve to a String
 * 
 * @param curve    The FadeCurve to convert to string
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