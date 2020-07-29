#ifndef NETWORK_H_INCLUDE
#define NETWORK_H_INCLUDE

#include <Arduino.h>
#include "PubSubClient.h"
#include <ESP8266WiFi.h>
#include "Enums.h"

namespace LedControllerSoftwareMk5
{

    class Network
    {
        // ## Data ## //
    private:
        WiFiClient wifiMqtt;

        unsigned long PrevMillis_WiFiTimeout = 0;
        unsigned long PrevMillis_MQTTTimeout = 0;
        unsigned long PrevMillis_HeartbeatTimeout = 0;
        const unsigned long TimeOut_WiFiTimeout = 5000; // 5 sec
        const unsigned long TimeOut_MQTTTimeout = 5000; // 5 sec
        const unsigned long TimeOut_HeartbeatTimeout = 5000; // 5 sec
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
        // ==== Parameter

        // == Global
        // Sun
        bool parameter_sun              = false;
        // Time
        uint8_t parameter_time_hour     = 12;
        uint8_t parameter_time_minute   = 0;
        // Master
        bool parameter_master_present   = false;
        // PC
        bool parameter_pc_present       = false;
        // Motion
        bool paramter_motion_detection_power    = false;
        uint8_t parameter_motion_red_value      = 255;
        uint8_t parameter_motion_green_value    = 128;
        uint8_t parameter_motion_blue_value     = 0;
        uint16_t parameter_motion_timeout       = 30;

        // == LED Strip 1
        bool parameter_led_strip_1_power                = false;
        uint8_t parameter_led_strip_1_brightness        = 0;
        uint8_t parameter_led_strip_1_cold_white_value  = 0;
        uint8_t parameter_led_strip_1_warm_white_value  = 0;
        uint8_t parameter_led_strip_1_red_value         = 0;
        uint8_t parameter_led_strip_1_green_value       = 0;
        uint8_t parameter_led_strip_1_blue_value        = 0;
        LEDEffect parameter_led_strip_1_effect          = LEDEffect::None;

        // == LED Strip 2
        bool parameter_led_strip_2_power                = false;
        uint8_t parameter_led_strip_2_brightness        = 0;
        uint8_t parameter_led_strip_2_cold_white_value  = 0;
        uint8_t parameter_led_strip_2_warm_white_value  = 0;
        uint8_t parameter_led_strip_2_red_value         = 0;
        uint8_t parameter_led_strip_2_green_value       = 0;
        uint8_t parameter_led_strip_2_blue_value        = 0;
        LEDEffect parameter_led_strip_2_effect          = LEDEffect::None;

        // ## Functions ## //
    private:
        void HandleWifi();
        void HandleMqtt();
        void Heartbeat();

    public:
        Network();
        void Init();
        void Run();
        void ElectricalMeasurementPowerUpdate(double powerValue);
        void ElectricalMeasurementVoltageUpdate(double voltageValue);
        void ElectricalMeasurementCurrentUpdate(double currentValue);
        void MotionDetectedUpdate(bool motion);
    };

};

#endif