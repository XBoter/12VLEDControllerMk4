#ifndef ENUMS_H_INCLUDE
#define ENUMS_H_INCLUDE

namespace LedControllerSoftwareMk5
{

    enum LEDEffect : int
    {
        NoMasterPresent = -999, // Cant be set via network
        MotionDetected  = - 10, // Cant be set via network
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

    enum WiFiState : int
    {
        StartWifi = 0,
        SuperviseWiFiConnection = 10,
        CheckWiFiDisconnect = 20,
    };

    enum MQTTState : int
    {
        StartMqtt = 0,
        SuperviseMqttConnection = 10,
        CheckMqttDisconnect = 20,
    };

};

#endif