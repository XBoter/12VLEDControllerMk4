#pragma once

// LED Effects that use multiple connected LED strips
enum class MultiLEDEffect
{
    NoMasterPresent, 
    Idle,
    Power,
    MotionDetected,
    Music,
    Alarm,
};


// LED Effects that are used by a singel LED strip
enum class SingleLEDEffect 
{
    None,
    TriplePulse,
    Rainbow
};


// Diffrent curves for fading a value
enum class FadeCurve 
{
    None,    // Instant => No fading at all
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
};


// Diffrent Alarm modes
enum class AlarmMode
{
    Nothing,
    Warning,
    Error,
    Critical,
};


// Enum for the wifi state
enum class NetworkWiFiState
{
    StartWifi,
    SuperviseWiFiConnection,
    CheckWiFiDisconnect,
};


// Enum for the mqtt state
enum class NetworkMQTTState
{
    StartMqtt,
    SuperviseMqttConnection,
    CheckMqttDisconnect,
};

