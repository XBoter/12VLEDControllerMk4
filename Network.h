#ifndef NETWORK_H_INCLUDE
#define NETWORK_H_INCLUDE

#include <Arduino.h>
#include "PubSubClient.h"
#include <ESP8266WiFi.h>

namespace LedControllerSoftwareMk5
{

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

    class Network
    {
        // ## Data ## //
    private:
        WiFiClient wifiMqtt;

        unsigned long PrevMillis_WiFiTimeout = 0;
        unsigned long PrevMillis_MQTTTimeout = 0;
        const unsigned long TimeOut_WiFiTimeout = 5000; // 5 sec
        const unsigned long TimeOut_MQTTTimeout = 5000; // 5 sec
        bool wifiOneTimePrint = true;
        bool mqttOneTimePrint = true;
        bool memWifiConnected = false;
        bool memMqttConnected = false;

    public:
        PubSubClient mqttClient;
        WiFiState wifiState = WiFiState::StartWifi;
        MQTTState mqttState = MQTTState::StartMqtt;
        bool wifiConnected = false;
        bool mqttConnected = false;

        // ## Functions ## //
    private:
        void HandleWifi();
        void HandleMqtt();

    public:
        Network();
        void Init();
        void Run();
    };

} // namespace LedControllerSoftwareMk5

#endif