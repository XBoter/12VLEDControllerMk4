#pragma once

/**
 * @brief Defines the possible Multi LED Strips effects
 * 
 */
enum class MultiLEDEffect
{
    Idle,
    SingleLEDEffect,
    MotionDetected,
    Alarm,
};

/**
 * @brief Defines the possible Single LED Strip effects
 * 
 */
enum class SingleLEDEffect
{
    None,
    TriplePulse,
    Rainbow
};

/**
 * @brief Defines the options for fading functions used by the LED Strips
 * 
 */
enum class FadeCurve
{
    None,
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
};

// Diffrent Alarm modes
/*
enum class AlarmMode
{
    Nothing,
    Warning,
    Error,
    Critical,
};
*/

/**
 * @brief Defines the WiFi Connection States
 * 
 */
enum class NetworkWiFiState
{
    StartWifi,
    SuperviseWiFiConnection,
    CheckWiFiDisconnect,
};

/**
 * @brief Defines the MQTT Connection States
 * 
 */
enum class NetworkMQTTState
{
    StartMqtt,
    SuperviseMqttConnection,
    CheckMqttDisconnect,
};

/**
 * @brief Defines the Output Options which 1 pin on the LED Strip connector can have
 * 
 */
enum class LEDOutputType
{
    R,
    G,
    B,
    CW,
    WW,
};
