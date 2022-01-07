#pragma once

// Arduino Lib Includes
#include <NTPClient.h>    // @installed via Arduino Library Manger    GitHub => https://github.com/arduino-libraries/NTPClient
#include <PubSubClient.h> // @installed via Arduino Library Manger    GitHub => https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>  // @installed via Arduino Library Manger    GitHub => https://github.com/bblanchon/ArduinoJson

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "../Enums/Enums.h"
#include "../Structs/Structs.h"
#include "../Constants/Constants.h"
#include "../Filesystem/Filesystem.h"
#include "../Helper/Helper.h"
#include "../Information/Information.h"
#include "../PirReader/PirReader.h"
#include "../Parameterhandler/Parameterhandler.h"
#include "../PowerMeasurement/PowerMeasurement.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Filesystem;
class Helper;
class Information;
class PirReader;
class PowerMeasurement;
class Parameterhandler;

// ================================ CLASS ================================ //
class Network : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Network(String codeVersion);
    void setReference(Filesystem *filesystem,
                      Helper *helper,
                      Information *information,
                      PirReader *pirReader,
                      PowerMeasurement *powerMeasurement,
                      Parameterhandler *parameterhandler);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    // ======== External Components ======== //
    Filesystem *filesystem;
    Helper *helper;
    Information *information;
    PirReader *pirReader;
    PowerMeasurement *powerMeasurement;
    Parameterhandler *parameterhandler;

    // ==== NTP
    unsigned long PrevMillis_NTPTimeout = 0;
    const unsigned long TimeOut_NTPTimeout = 60000; // 1 minute
    NetworkNTPState ntpState = NetworkNTPState::StartNTP;
    NetworkNTPState memNtpState = NetworkNTPState::StartNTP;
    const long utcOffsetInSeconds = 3600; // UTC +1 (Germany) => 1 * 60 * 60 => 3600
    WiFiUDP ntpUDP;
    NTPClient ntpTimeClient = NTPClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

    // ==== MQTT
    unsigned long PrevMillis_MQTTTimeout = 0;
    const unsigned long TimeOut_MQTTTimeout = 5000; // 5 sec
    NetworkMQTTState mqttState = NetworkMQTTState::StartMQTT;
    NetworkMQTTState memMqttState = NetworkMQTTState::StartMQTT;
    PubSubClient mqttClient;
    bool MQTTConnected = false;
    int clientState = 0;
    WiFiClient wifiMqtt;
    DynamicJsonDocument doc = DynamicJsonDocument(2048);

    // ==== WiFi
    unsigned long PrevMillis_WiFiTimeout = 0;
    const unsigned long TimeOut_WiFiTimeout = 5000; // 5 sec
    NetworkWiFiState wifiState = NetworkWiFiState::StartWiFi;
    NetworkWiFiState memWifiState = NetworkWiFiState::StartWiFi;
    bool WiFiConnected = false;
    bool wiFiMode = true;
    bool shutdownWiFi = false;
    bool changeToWiFiModeRequest = false;

    // ==== Access Point
    NetworkAccessPointState accessPointState = NetworkAccessPointState::IdleAccessPoint;
    bool accessPointReady = false;
    bool accessPointMode = false;
    bool shutdownAccessPoint = true;
    bool changeToAccessPointModeRequest = false;
    uint8_t accessPointConnectedClients = 0;
    String accesspointName = "LED Controller Mk4";
    IPAddress accessPointIPAddress = IPAddress(192, 168, 1, 1);
    IPAddress accessPointSubnetmask = IPAddress(255, 255, 255, 0);

    // ==== Information Print
    bool motionDetectionDataPrint = false;
    bool ledStripDataPrint[STRIP_COUNT]{false};

    // ====  Data
    NetworkMotionData networkMotionData = {};
    NetworkLEDStripData networkLEDStripData[STRIP_COUNT] = {};
    DetailedSunData detailedSunData = {};
    NetworkTimeData networkTimeData = {};

    // ==== Republish / Publish functions
    unsigned long prevMillisPublishMotionDetected = 0;
    unsigned long prevMillisPublishElectricalMeasurement = 0;
    unsigned long prevMillisPublishHeartbeat = 0;
    unsigned long prevMillisPublishNetwork = 0;

    uint32_t timeoutPublishMotionDetected = 60000;        // 1 Minute
    uint32_t timeoutPublishElectricalMeasurement = 60000; // 1 Minute
    uint32_t timeoutPublishHeartbeat = 5000;              // 5 Seconds
    uint32_t timeoutPublishNetwork = 60000;               // 1 Minute

    // ======== Other ======== //
    String codeVersion = "";

public:
    // ================ Methods ================ //
private:
    // ==== Network
    void HandleWiFi(bool shutdown);
    void HandleAccessPoint(bool shutdown);
    void HandleMqtt();
    void HandleNTP();
    void MqttCallback(char *topic, byte *payload, unsigned int length);

    // ==== Republish / Publish functions
    void HandleRepublish();

    void PublishMotionDetected();
    void PublishElectricalMeasurement();
    void PublishHeartbeat();
    void PublishMotionLEDStripData();
    void PublishNetwork();
    void PublishCodeVersion();

public:
    bool isWiFiConnected();
    bool isAccessPointReady();
    bool isMQTTConnected();

    void RequestChangeToWiFiMode();
    bool isInWiFiMode();
    void RequestChangeToAccessPointMode();
    bool isInAccessPointMode();

    NetworkWiFiInformation getWiFiInformation();
    NetworkMQTTInformation getMQTTInformation();

    bool isVirtualPIRSensorTriggered();
    void resetVirtualPIRSensor();
    bool isMasterPresent(uint8_t stripID);
    bool isAlarm(uint8_t stripID);
    bool isSunUnderTheHorizon();

    NetworkMotionData getNetworkMotionData();
    void UpdateNetworkMotionData(NetworkMotionData data, bool republish = false);

    NetworkLEDStripData getNetworkLEDStripData(uint8_t stripID);
    void UpdateNetworkLEDStripData(uint8_t stripID, NetworkLEDStripData data, bool republish = false);

    DetailedSunData getDetailedSunData();
    NetworkTimeData getNetworkTimeData();
};
