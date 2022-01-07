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
 * @brief The raw Data that gets used by the LED Strip
 * 
 */
struct RawLEDStripData
{
    // ==== Refresh Rate
    unsigned long lastRefreshRateCount = 0; // Needed to check if FadeColor got called every refresh cycle or if skipped

    // ==== RED
    uint8_t redColorValue = 0;
    uint8_t prevRedColorValue = 0;
    unsigned long prevMillisRedColorFade = 0;
    uint16_t redBrightnessValue = 0;
    uint16_t prevRedBrightnessValue = 0;
    unsigned long prevMillisRedBrightnessFade = 0;

    // ==== GREEN
    uint8_t greenColorValue = 0;
    uint8_t prevGreenColorValue = 0;
    unsigned long prevMillisGreenColorFade = 0;
    uint16_t greenBrightnessValue = 0;
    uint16_t prevGreenBrightnessValue = 0;
    unsigned long prevMillisGreenBrightnessFade = 0;

    // ==== BLUE
    uint8_t blueColorValue = 0;
    uint8_t prevBlueColorValue = 0;
    unsigned long prevMillisBlueColorFade = 0;
    uint16_t blueBrightnessValue = 0;
    uint16_t prevBlueBrightnessValue = 0;
    unsigned long prevMillisBlueBrightnessFade = 0;

    // ==== CW
    uint8_t cwColorValue = 0;
    uint8_t prevCwColorValue = 0;
    unsigned long prevMillisCwColorFade = 0;
    uint16_t cwBrightnessValue = 0;
    uint16_t prevCwBrightnessValue = 0;
    unsigned long prevMillisCwBrightnessFade = 0;

    // ==== WW
    uint8_t wwColorValue = 0;
    uint8_t prevWwColorValue = 0;
    unsigned long prevMillisWwColorFade = 0;
    uint16_t wwBrightnessValue = 0;
    uint16_t prevWwBrightnessValue = 0;
    unsigned long prevMillisWwBrightnessFade = 0;

    // ==== FLAGS
    bool fadeFinished = false;
};

/**
 * @brief The basic led strip data type
 * 
 */
struct LEDBasicStripData
{
    uint8_t colorValue = 0;
    uint16_t brightnessValue = 0;
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

// ================================================ Settings ================================================ //
struct SettingsStripParameter
{
    // ==== OUTPUT TYPE
    LEDOutputType ChannelOutputType[CHANNEL_COUNT]{LEDOutputType::R};
};

struct FilesystemSettingsStripData : public SettingsStripParameter
{
    bool isConfigured = false;
};

// ================================================ Configuration ================================================ //
// We use a char array here, so that we can easy save/load the data to the LittleFS system
struct ConfigurationParameter
{
    String WiFiSSID = "unknown";
    String WiFiPassword = "unknown";
    String MQTTBrokerIpAddress = "unknown";
    uint16_t MQTTBrokerPort = 0;
    String MQTTBrokerUsername = "unknown";
    String MQTTBrokerPassword = "unknown";
    String MQTTClientName = "unknown";
};

struct FilesystemConfigurationData : public ConfigurationParameter
{
    bool isConfigured = false;
    bool isFullyConfigured = false;
    bool isStandaloneMode = false;
};

// ================================================ Motion ================================================ //
struct MotionParameter
{
    // ==== MOTION
    bool MotionDetectionEnabled = false;
    bool TimeBasedBrightnessChangeEnabled = false;
    uint16_t MotionDetectionTimeout = 0;
    // ==== MOTION RGB
    uint8_t Red = 0;
    uint8_t Green = 0;
    uint8_t Blue = 0;
    uint16_t ColorFadeTime = 0;
    FadeCurve ColorFadeCurve = FadeCurve::None;
    uint16_t ColorBrightness = 0;
    uint16_t ColorBrightnessFadeTime = 0;
    FadeCurve ColorBrightnessFadeCurve = FadeCurve::None;
    // ==== MOTION WHITE
    uint16_t WhiteTemperature = 250;
    uint16_t WhiteTemperatureFadeTime = 0;
    FadeCurve WhiteTemperatureFadeCurve = FadeCurve::None;
    uint16_t WhiteTemperatureBrightness = 0;
    uint16_t WhiteTemperatureBrightnessFadeTime = 0;
    FadeCurve WhiteTemperatureBrightnessFadeCurve = FadeCurve::None;
};

struct FilesystemMotionData : public MotionParameter
{
    bool isConfigured = false;
};

struct NetworkMotionData : public MotionParameter
{
    bool SunUnderTheHorizon = false;
    bool VirtualPIRSensorTriggered = false;
};

// ================================================ LED Strip ================================================ //
struct LEDStripParameter
{
    // ==== POWER
    bool Power = false;
    // ==== RGB
    uint8_t Red = 0;
    uint8_t Green = 0;
    uint8_t Blue = 0;
    uint16_t ColorFadeTime = 0;
    FadeCurve ColorFadeCurve = FadeCurve::None;
    uint16_t ColorBrightness = 0;
    uint16_t ColorBrightnessFadeTime = 0;
    FadeCurve ColorBrightnessFadeCurve = FadeCurve::None;
    // ==== WHITE
    uint16_t WhiteTemperature = 250;
    uint16_t WhiteTemperatureFadeTime = 0;
    FadeCurve WhiteTemperatureFadeCurve = FadeCurve::None;
    uint16_t WhiteTemperatureBrightness = 0;
    uint16_t WhiteTemperatureBrightnessFadeTime = 0;
    FadeCurve WhiteTemperatureBrightnessFadeCurve = FadeCurve::None;
    // ==== EFFECT
    SingleLEDEffect Effect = SingleLEDEffect::None;
};

struct FilesystemLEDStripData : public LEDStripParameter
{
    bool isConfigured = false;
};

struct NetworkLEDStripData : public LEDStripParameter
{
    bool MasterPresent = false;
    bool AlarmActive = false;
};