#pragma once

// Enum with all the different LED Effects possible
enum LEDEffect : int
{
    NoMasterPresent = -999, 
    MotionDetected  = - 10, 
    None            = 0,
    Alarm           = 10,
    Music           = 20,
    Sleep           = 30,
    Weekend         = 40,
    RGB             = 50,
    CW              = 60,
    WW              = 70,
    RGBCW           = 80,
    RGBWW           = 90,
    CWWW            = 100,
};

// Enum for the wifi state
enum NetworkWiFiState : int
{
    StartWifi               = 0,
    SuperviseWiFiConnection = 10,
    CheckWiFiDisconnect     = 20,
};

// Enum for the mqtt state
enum NetworkMQTTState : int
{
    StartMqtt               = 0,
    SuperviseMqttConnection = 10,
    CheckMqttDisconnect     = 20,
};

