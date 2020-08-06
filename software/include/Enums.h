#pragma once

// Diffrent modes of the controller
enum class ControllerMode
{
    NoMasterPresent, 
    Idle,
    Power,
    MotionDetected,
};


// Diffrent LED Effects
enum class LEDEffect 
{
    None,
    Alarm,
    Music,
    Sleep,
    Weekend,
    RGB,
    CW,
    WW,
    RGBCW,
    RGBWW,
    CWWW, 
};


// Diffrent curves for fading a value
enum class FadeCurve 
{
    None,    // Instant => No fading at all
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    EaseOutIn, 
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

