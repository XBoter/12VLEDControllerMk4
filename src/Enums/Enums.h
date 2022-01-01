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

/**
 * @brief Defines the webserver configuration mode state
 * 
 */
enum class WebserverConfigurationModeState
{
    StartConfigurationMode,
    RunConfigurationMode,
    ShutdownConfigurationMode,
    IdleConfigurationMode,
};

/**
 * @brief A substate for the configuration mode 
 * 
 */
enum class WebserverConfigurationModeSubState
{
    BeginWebserver,
    HandleClients,
    StopWebServer,
};

/**
 * @brief Defines the webserver normal mode state
 * 
 */
enum class WebserverNormalModeState
{
    StartNormalMode,
    RunNormalMode,
    ShutdownNormalMode,
    IdleNormalMode,
};

/**
 * @brief A substate for the normal mode 
 * 
 */
enum class WebserverNormalModeSubState
{
    BeginWebserver,
    HandleClients,
    StopWebServer,
};

/**
 * @brief Defines the WiFi Connection States
 * 
 */
enum class NetworkWiFiState
{
    StartWiFi,
    SuperviseWiFiConnection,
    CheckWiFiDisconnect,
    ShutdownWiFi,
    IdleWiFi,
};

enum class NetworkAccessPointState
{
    StartAccessPoint,
    SuperviseAccessPointConnection,
    ShutdownAccessPoint,
    IdleAccessPoint,
};

/**
 * @brief Defines the MQTT Connection States
 * 
 */
enum class NetworkMQTTState
{
    StartMQTT,
    SuperviseMQTTConnection,
    CheckMQTTDisconnect,
};

/**
 * @brief Defines the NTP Connection States
 * 
 */
enum class NetworkNTPState
{
    StartNTP,
    SuperviseNTPConnection,
    CheckNTPDisconnect,
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
