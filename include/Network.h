#pragma once

// Arduino Lib Includes
#include <NTPClient.h>      // @installed via Arduino Library Manger    GitHub => https://github.com/arduino-libraries/NTPClient
#include <PubSubClient.h>   // @installed via Arduino Library Manger    GitHub => https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>    // @installed via Arduino Library Manger    GitHub => https://github.com/bblanchon/ArduinoJson

// Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Enums.h"
#include "Structs.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class Network : public IBaseClass
{
    // ## Constructor ## //
    public:
        Network();

    // ## Interface ## //
    private:
    public:
        virtual bool Init();
        virtual void Run();

    // ## Data ## //
    private:
        WiFiClient wifiMqtt;
        WiFiUDP ntpUDP;
        const long utcOffsetInSeconds = 3600; // UTC +1 (Germany) => 1 * 60 * 60 => 3600
        NTPClient timeClient = NTPClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

        // Prev Millis
        unsigned long PrevMillis_WiFiTimeout = 0;
        unsigned long PrevMillis_MQTTTimeout = 0;
        unsigned long PrevMillis_NTPTimeout = 0;
        unsigned long PrevMillis_HeartbeatTimeout = 0;
        
        // Timeout
        const unsigned long TimeOut_WiFiTimeout = 5000;      // 5 sec
        const unsigned long TimeOut_MQTTTimeout = 5000;      // 5 sec
        const unsigned long TimeOut_NTPTimeout = 400;        // 0. sec
        const unsigned long TimeOut_HeartbeatTimeout = 5000; // 5 sec
        
        bool wifiOneTimePrint = true;
        bool mqttOneTimePrint = true;
        bool memWifiConnected = false;
        bool memMqttConnected = false;

    public:
        DynamicJsonDocument doc = DynamicJsonDocument(2048);
        PubSubClient mqttClient;
        NetworkWiFiState wifiState = NetworkWiFiState::StartWifi;
        NetworkMQTTState mqttState = NetworkMQTTState::StartMqtt;
        bool wifiConnected = false;
        bool mqttConnected = false;
       
        // MQTT Data
        // Sun
        bool parameter_sun = false;
        // Time
        TimeData stTimeData;
        // Master
        bool parameter_master_present = false;
        // PC
        bool parameter_pc_present = false;
        // Motion
        MotionData stMotionData;
        // LED Strip 1
        LEDStripData stLedStrip1Data;
        // LED Strip 2
        LEDStripData stLedStrip2Data;

    // ## Functions ## //
    private:
        // Network handles
        void HandleWifi(); 
        void HandleMqtt();
        void HandleNTP();

        void Heartbeat();
        void MqttUpdateAfterDc(LEDStripData ledStripData,
                               const char *topic);

    public:
        void ElectricalMeasurementUpdate(double currentPower,
                                         double busVoltage,
                                         double busCurrent);
        void MotionDetectedUpdate(bool motion);
};
