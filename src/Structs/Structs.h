#pragma once

#include "../Enums/Enums.h"
#include "../Constants/Constants.h"

/**
 * Holds the data for the current time
 */
struct NetworkTimeData
{
    uint8_t hour = 12;
    uint8_t minute = 0;
    uint8_t second = 0;
    unsigned long unix = 0;
};

/**
 * @brief Holds information about the current MQTT connection
 * 
 */
struct NetworkMQTTInformation
{
    String clientName = "unknown";
    String brokerIpAddress = "unknown";
    uint16_t brokerPort = 0;
    int clientState = 0;
    bool isMQTTConnected = false;
};

/**
 * @brief Holds information about the current usage of the WiFi connection
 * 
 */
struct NetworkWiFiInformation
{
    String ipAddress = "unknown";
    String subnetMask = "unknown";
    String macAddress = "unknown";
    String gatewayIpAddress = "unknown";
    String hostname = "unknown";
    bool inWiFiMode = false;
    bool isWiFiConnected = false;
    bool inAccessPointMode = false;
};

/**
 * @brief Holds detailed information about the sunfall and sunrise
 * 
 */
struct DetailedSunData
{
    // Sunfall
    unsigned long sunfallUnix = 0;
    bool isSunfallSet = false;

    // Sunrise
    unsigned long sunriseUnix = 0;
    bool isSunriseSet = false;
};

/**
 * Holds the data for the motion detection
 */
struct NetworkMotionData
{
    bool motionDetectionEnabled = false;
    bool timeBasedBrightnessChangeEnabled = false;
    uint16_t timeout = 0;
    // RGB
    uint8_t redColorValue = 0;
    uint8_t greenColorValue = 0;
    uint8_t blueColorValue = 0;
    uint16_t colorBrightnessValue = 0;
    // White
    uint16_t whiteTemperatureValue = 0;
    uint16_t whiteBrightnessValue = 0;
};

/**
 * Holds all register for one LED Color
 */
struct LEDColorReg
{
    uint8_t ON_L = 0x00;
    uint8_t ON_H = 0x00;
    uint8_t OFF_L = 0x00;
    uint8_t OFF_H = 0x00;
};

/**
 * Holds all register for one LED strip
 */
struct LEDStripColorReg
{
    LEDColorReg RED_REG = {};
    LEDColorReg GREEN_REG = {};
    LEDColorReg BLUE_REG = {};
    LEDColorReg CW_REG = {};
    LEDColorReg WW_REG = {};
};

/**
 * Holds data for LED effects that use multiple connected LED strips
 */
struct MultiLEDStripEffectData
{
    MultiLEDEffect multiLEDEffect = MultiLEDEffect::Idle;
    uint8_t transitionState = 0;
    uint8_t subTransitionState = 0;
    uint8_t counter = 0;
    bool fadeFinished = false;
    unsigned long prevMillis = 0;
};

/**
 * Holds data for LED effects that are only used by one LED strip
 */
struct SingleLEDStripEffectData
{
    SingleLEDEffect singleLEDEffect = SingleLEDEffect::None;
    uint8_t transitionState = 0;
    uint8_t subTransitionState = 0;
    uint8_t counter = 0;
    bool fadeFinished = false;
    unsigned long prevMillis = 0;
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
    // ---- RGB
    // -- Color
    uint8_t redColorValue = 0;
    uint8_t greenColorValue = 0;
    uint8_t blueColorValue = 0;
    uint16_t colorFadeTime = 0;
    FadeCurve colorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t colorBrightnessValue = 0;
    uint16_t colorBrightnessFadeTime = 0;
    FadeCurve colorBrightnessFadeCurve = FadeCurve::None;

    // ---- White
    // -- Color
    uint16_t whiteTemperatureValue = 250;
    uint16_t whiteTemperatureFadeTime = 0;
    FadeCurve whiteTemperatureFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t whiteBrightnessValue = 0;
    uint16_t whiteBrightnessFadeTime = 0;
    FadeCurve whiteBrightnessFadeCurve = FadeCurve::None;
};

/**
 * The LED strip data received via mqtt
 */
struct NetworkLEDStripData
{
    bool power = false;
    HighLevelLEDStripData ledStripData;
    SingleLEDEffect effect = SingleLEDEffect::None;
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
    uint8_t redColorValue = 0;
    uint16_t redColorFadeTime = 0;
    FadeCurve redColorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t redBrightnessValue = 0;
    uint16_t redBrightnessFadeTime = 0;
    FadeCurve redBrightnessFadeCurve = FadeCurve::None;

    // ---- GREEN
    // -- Color
    uint8_t greenColorValue = 0;
    uint16_t greenColorFadeTime = 0;
    FadeCurve greenColorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t greenBrightnessValue = 0;
    uint16_t greenBrightnessFadeTime = 0;
    FadeCurve greenBrightnessFadeCurve = FadeCurve::None;

    // ---- BLUE
    // -- Color
    uint8_t blueColorValue = 0;
    uint16_t blueColorFadeTime = 0;
    FadeCurve blueColorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t blueBrightnessValue = 0;
    uint16_t blueBrightnessFadeTime = 0;
    FadeCurve blueBrightnessFadeCurve = FadeCurve::None;

    // ---- CW
    // -- Color
    uint8_t cwColorValue = 0;
    uint16_t cwColorFadeTime = 0;
    FadeCurve cwColorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t cwBrightnessValue = 0;
    uint16_t cwBrightnessFadeTime = 0;
    FadeCurve cwBrightnessFadeCurve = FadeCurve::None;

    // ---- WW
    // -- Color
    uint8_t wwColorValue = 0;
    uint16_t wwColorFadeTime = 0;
    FadeCurve wwColorFadeCurve = FadeCurve::None;
    // -- Brightness
    uint16_t wwBrightnessValue = 0;
    uint16_t wwBrightnessFadeTime = 0;
    FadeCurve wwBrightnessFadeCurve = FadeCurve::None;
};

/**
 * Hold the actual values of the LED strip and prev fade times
 */
struct LEDStripData
{
    // ---- Refresh Rate
    unsigned long lastRefreshRateCount = 0; // Needed to check if FadeColor got called every refresh cycle or if skipped

    // ---- RED
    // -- Color
    uint8_t redColorValue = 0;
    uint8_t prevRedColorValue = 0;
    unsigned long prevMillisRedColorFade = 0;
    // -- Brightness
    uint16_t redBrightnessValue = 0;
    uint16_t prevRedBrightnessValue = 0;
    unsigned long prevMillisRedBrightnessFade = 0;

    // ---- GREEN
    // -- Color
    uint8_t greenColorValue = 0;
    uint8_t prevGreenColorValue = 0;
    unsigned long prevMillisGreenColorFade = 0;
    // -- Brightness
    uint16_t greenBrightnessValue = 0;
    uint16_t prevGreenBrightnessValue = 0;
    unsigned long prevMillisGreenBrightnessFade = 0;

    // ---- BLUE
    // -- Color
    uint8_t blueColorValue = 0;
    uint8_t prevBlueColorValue = 0;
    unsigned long prevMillisBlueColorFade = 0;
    // -- Brightness
    uint16_t blueBrightnessValue = 0;
    uint16_t prevBlueBrightnessValue = 0;
    unsigned long prevMillisBlueBrightnessFade = 0;

    // ---- CW
    // -- Color
    uint8_t cwColorValue = 0;
    uint8_t prevCwColorValue = 0;
    unsigned long prevMillisCwColorFade = 0;
    // -- Brightness
    uint16_t cwBrightnessValue = 0;
    uint16_t prevCwBrightnessValue = 0;
    unsigned long prevMillisCwBrightnessFade = 0;

    // ---- WW
    // -- Color
    uint8_t wwColorValue = 0;
    uint8_t prevWwColorValue = 0;
    unsigned long prevMillisWwColorFade = 0;
    // -- Brightness
    uint16_t wwBrightnessValue = 0;
    uint16_t prevWwBrightnessValue = 0;
    unsigned long prevMillisWwBrightnessFade = 0;

    // ---- INFO
    bool fadeFinished = false;
};

/**
 * @brief Data Type which holds all cirtical information needed for Network connectivity
 * 
 */
struct ConfigurationData
{
    String wifiSSID = "";
    String wifiPassword = "";
    String mqttBrokerIpAddress = "";
    uint16_t mqttBrokerPort = 0;
    String mqttBrokerUsername = "";
    String mqttBrokerPassword = "";
    String mqttClientName = "";
    // == Flags
    bool isConfigured = false; // If true the data is configured initially
};

/**
 * @brief Data Type which holds the settings data for example the hardware LED Strip output config
 * 
 */
struct SettingsData
{
    // == LED Strip 1 Output Config
    LEDOutputType ledStrip1Output1 = LEDOutputType::R;
    LEDOutputType ledStrip1Output2 = LEDOutputType::G;
    LEDOutputType ledStrip1Output3 = LEDOutputType::B;
    LEDOutputType ledStrip1Output4 = LEDOutputType::CW;
    LEDOutputType ledStrip1Output5 = LEDOutputType::WW;
    // == LED Strip 2 Output Config
    LEDOutputType ledStrip2Output1 = LEDOutputType::R;
    LEDOutputType ledStrip2Output2 = LEDOutputType::G;
    LEDOutputType ledStrip2Output3 = LEDOutputType::B;
    LEDOutputType ledStrip2Output4 = LEDOutputType::CW;
    LEDOutputType ledStrip2Output5 = LEDOutputType::WW;
    // == Flags
    bool isConfigured = false; // If true the data is configured initially
};

/**
 * @brief Data Type which holds the last state of the LED Strip
 * 
 */
struct LEDStateData
{
    // == LED Strip 1 State
    bool ledStrip1Power = false;
    uint8_t ledStrip1RedValue = 0;
    uint8_t ledStrip1GreenValue = 0;
    uint8_t ledStrip1BlueValue = 0;
    uint8_t ledStrip1ColdWhiteValue = 0;
    uint8_t ledStrip1WarmWhiteValue = 0;
    uint8_t ledStrip1BrightnessValue = 0;
    SingleLEDEffect ledStrip1EffectValue = SingleLEDEffect::None;
    // == LED Strip 2 State
    bool ledStrip2Power = false;
    uint8_t ledStrip2RedValue = 0;
    uint8_t ledStrip2GreenValue = 0;
    uint8_t ledStrip2BlueValue = 0;
    uint8_t ledStrip2ColdWhiteValue = 0;
    uint8_t ledStrip2WarmWhiteValue = 0;
    uint8_t ledStrip2BrightnessValue = 0;
    SingleLEDEffect ledStrip2EffectValue = SingleLEDEffect::None;
    // == Flags
    bool isConfigured = false; // If true the data is configured initially
};

/**
 * @brief Data Type which holds information about the PIR Reader
 * 
 */
struct PIRReaderData
{
    bool motionDetected = false;
    bool sensorTriggered = false;
    bool sensor1Triggered = false;
    bool sensor2Triggered = false;
    bool virtualSensorTriggered = false;
};

/**
 * @brief Holds all the data received/transmitted over MQTT needed for the led operations
 * 
 */
struct NetworkData
{
    bool masterPresent = false;
    bool sunUnderTheHorizon = false;
    bool alarm = false;
    bool virtualPIRSensorTriggered = false;
    NetworkMotionData networkMotionData = {};
    NetworkLEDStripData networkLEDStripData[STRIP_COUNT] = {};
};