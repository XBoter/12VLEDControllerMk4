#pragma once

// Arduino Lib Includes
#include <NTPClient.h>    // @installed via Arduino Library Manger    GitHub => https://github.com/arduino-libraries/NTPClient
#include <PubSubClient.h> // @installed via Arduino Library Manger    GitHub => https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>  // @installed via Arduino Library Manger    GitHub => https://github.com/bblanchon/ArduinoJson

// Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Enums.h"
#include "Structs.h"
#include "Configuration.h"
#include "Information.h"
#include "PirReader.h"
#include "PowerMeasurement.h"

// Interface
#include "../interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Configuration;
class Information;
class PirReader;
class PowerMeasurement;

// Classes
class Network : public IBaseClass
{
    // ## Constructor ## //
public:
    Network();
    void setReference(Configuration *configuration,
                      Information *information,
                      PirReader *pirReader,
                      PowerMeasurement *powerMeasurement);

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    // External components
    Configuration *configuration;
    Information *information;
    PirReader *pirReader;
    PowerMeasurement *powerMeasurement;

    ConfiguredData data = {};

    WiFiClient wifiMqtt;
    WiFiUDP ntpUDP;
    const long utcOffsetInSeconds = 3600; // UTC +1 (Germany) => 1 * 60 * 60 => 3600
    NTPClient timeClient = NTPClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

    // Network Info
    String ipAddress = "";
    String macAddress = "";

    // Prev Millis
    unsigned long PrevMillis_WiFiTimeout = 0;
    unsigned long PrevMillis_MQTTTimeout = 0;
    unsigned long PrevMillis_NTPTimeout = 0;
    unsigned long PrevMillis_HeartbeatTimeout = 0;

    // Timeout
    const unsigned long TimeOut_WiFiTimeout = 5000;      // 5 sec
    const unsigned long TimeOut_MQTTTimeout = 5000;      // 5 sec
    const unsigned long TimeOut_NTPTimeout = 60000;      // 60 sec
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

    // Other Data
    TimeBasedMotionBrightness stTimeBasedMotionBrightness = {};

    // MQTT Data
    // Sun
    bool parameter_sun = false;
    // Time
    NetworkTimeData stNetworkTimeData = {};
    // Master
    bool parameter_master_present = false;
    // Alarm
    bool alarm = false;
    // Motion
    NetworkMotionData stNetworkMotionData = {};
    // LED Strip 1
    NetworkLEDStripData stNetworkLedStrip1Data = {};
    // LED Strip 2
    NetworkLEDStripData stNetworkLedStrip2Data = {};
    // Virtual PIR Sensor
    bool virtualPIRSensorTriggered = false;

    // ==== Republish / Publish functions
    unsigned long prevMillisPublishMotionDetected = 0;
    unsigned long prevMillisPublishLEDStripData = 0;
    unsigned long prevMillisPublishElectricalMeasurement = 0;
    unsigned long prevMillisPublishHeartbeat = 0;
    unsigned long prevMillisPublishMotionLEDStripData = 0;
    unsigned long prevMillisPublishNetwork = 0;

    uint32_t timeoutPublishMotionDetected = 10000;        // 10 Seconds
    uint32_t timeoutPublishLEDStripData = 300000;         // 5 Minutes
    uint32_t timeoutPublishElectricalMeasurement = 30000; // 30 Seconds
    uint32_t timeoutPublishHeartbeat = 5000;              // 5 Seconds
    uint32_t timeoutPublishMotionLEDStripData = 300000;   // 5 Minutes
    uint32_t timeoutPublishNetwork = 600000;              // 10 Minutes

    // ## Functions ## //
private:
    // Network handles
    void HandleWifi();
    void HandleMqtt();
    void HandleNTP();

    void MqttCallback(char *topic, byte *payload, unsigned int length);

    // ==== Republish / Publish functions
    // == Handle
    void HandleRepublish();

    // == Homeassistant
    void PublishHomeassistantMotionDetected();
    void PublishHomeassistantLEDStripData();
    void PublishHomeassistantElectricalMeasurement();
    void PublishHomeassistantHeartbeat();
    void PublishHomeassistantMotionLEDStripData();
    void PublishHomeassistantNetwork();

    // == Json
    void PublishJsonMotionDetected();
    void PublishJsonLEDStripData();
    void PublishJsonElectricalMeasurement();
    void PublishJsonHeartbeat();
    void PublishJsonMotionLEDStripData();
    void PublishJsonNetwork();

public:
    // ==== Republish / Publish functions
    void PublishMotionDetected();
    void PublishLEDStripData();
    void PublishElectricalMeasurement();
    void PublishHeartbeat();
    void PublishMotionLEDStripData();
    void PublishNetwork();
};
