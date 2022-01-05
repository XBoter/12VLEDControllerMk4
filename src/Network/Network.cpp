#include "Network.h"

/**
 * @brief Construct a new Network::Network object
 * 
 */
Network::Network(String codeVersion)
{
    this->codeVersion = codeVersion;
};

/**
 * @brief Sets the needed refernce for the helper
 */
void Network::setReference(Filesystem *filesystem,
                           Helper *helper,
                           Information *information,
                           PirReader *pirReader,
                           PowerMeasurement *powerMeasurement,
                           Parameterhandler *parameterhandler)
{
    this->filesystem = filesystem;
    this->helper = helper;
    this->information = information;
    this->pirReader = pirReader;
    this->powerMeasurement = powerMeasurement;
    this->parameterhandler = parameterhandler;
};

/**
 * @brief Initializes the network component
 * 
 * @return True if the initialization was successful
 */
bool Network::Init()
{
    if (!init)
    {
        this->wifiState = NetworkWiFiState::StartWiFi;
        this->mqttState = NetworkMQTTState::StartMQTT;
        this->ntpState = NetworkNTPState::StartNTP;

        this->networkData.virtualPIRSensorTriggered = false;

        Serial.println(F("Network initialized"));
        init = true;
    }

    return init;
};

/**
 * @brief Runs the network component
 * 
 */
void Network::Run()
{
    if (!init)
    {
        return;
    }

    // ==== STA / AP MODE ==== //
    if (this->changeToWiFiModeRequest)
    {
        this->shutdownAccessPoint = true;
        this->shutdownWiFi = true;
        if (this->accessPointState == NetworkAccessPointState::IdleAccessPoint)
        {
            this->wiFiMode = true;
            this->accessPointMode = false;
            this->shutdownWiFi = false;
            this->changeToWiFiModeRequest = false;
        }
    }
    else if (this->changeToAccessPointModeRequest)
    {
        this->shutdownAccessPoint = true;
        this->shutdownWiFi = true;
        if (this->wifiState == NetworkWiFiState::IdleWiFi)
        {
            this->wiFiMode = false;
            this->accessPointMode = true;
            this->shutdownAccessPoint = false;
            this->changeToAccessPointModeRequest = false;
        }
    }
    HandleAccessPoint(this->shutdownAccessPoint);
    HandleWiFi(this->shutdownWiFi);

    // ==== MQTT ==== //
    mqttClient.loop();
    HandleMqtt();

    // ==== NTP ==== //
    HandleNTP();

    // ==== REPUBLISH ==== //
    HandleRepublish();

    // ======== Information Print ======== //
    if (this->motionDetectionDataPrint)
    {
        this->motionDetectionDataPrint = false;
        this->information->FormatPrintMotion("Motion Parameter",
                                             this->helper->BoolToString(this->getNetworkMotionData().motionDetectionEnabled),
                                             this->helper->BoolToString(this->getNetworkMotionData().timeBasedBrightnessChangeEnabled),
                                             String(this->getNetworkMotionData().timeout),
                                             String(this->getNetworkMotionData().redColorValue),
                                             String(this->getNetworkMotionData().greenColorValue),
                                             String(this->getNetworkMotionData().blueColorValue),
                                             String(this->getNetworkMotionData().colorBrightnessValue),
                                             String(this->getNetworkMotionData().whiteTemperatureValue),
                                             String(this->getNetworkMotionData().whiteBrightnessValue));
    }
    if (this->ledStripDataPrint[0])
    {
        this->ledStripDataPrint[0] = false;
        this->information->FormatPrintLEDStrip("LED Strip 1",
                                               this->helper->BoolToString(this->getNetworkLEDStripData(1).power),
                                               String(this->getNetworkLEDStripData(1).ledStripData.redColorValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.greenColorValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.blueColorValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.colorFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ledStripData.colorFadeCurve),
                                               String(this->getNetworkLEDStripData(1).ledStripData.colorBrightnessValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.colorBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ledStripData.colorBrightnessFadeCurve),
                                               String(this->getNetworkLEDStripData(1).ledStripData.whiteTemperatureValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.whiteTemperatureFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ledStripData.whiteTemperatureFadeCurve),
                                               String(this->getNetworkLEDStripData(1).ledStripData.whiteBrightnessValue),
                                               String(this->getNetworkLEDStripData(1).ledStripData.whiteBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ledStripData.whiteBrightnessFadeCurve),
                                               this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(1).effect));
    }
    if (this->ledStripDataPrint[1])
    {
        this->ledStripDataPrint[1] = false;
        this->information->FormatPrintLEDStrip("LED Strip 2",
                                               this->helper->BoolToString(this->getNetworkLEDStripData(2).power),
                                               String(this->getNetworkLEDStripData(2).ledStripData.redColorValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.greenColorValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.blueColorValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.colorFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ledStripData.colorFadeCurve),
                                               String(this->getNetworkLEDStripData(2).ledStripData.colorBrightnessValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.colorBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ledStripData.colorBrightnessFadeCurve),
                                               String(this->getNetworkLEDStripData(2).ledStripData.whiteTemperatureValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.whiteTemperatureFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ledStripData.whiteTemperatureFadeCurve),
                                               String(this->getNetworkLEDStripData(2).ledStripData.whiteBrightnessValue),
                                               String(this->getNetworkLEDStripData(2).ledStripData.whiteBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ledStripData.whiteBrightnessFadeCurve),
                                               this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(2).effect));
    }
};

/**
 * @brief Handles the access point
 * 
 * @param shutdown If true the access point handler will shutdown the access point and change into the idle state
 */
void Network::HandleAccessPoint(bool shutdown)
{

    bool ret = false;

    switch (this->accessPointState)
    {
        // ================================ StartAccessPoint ================================ //
    case NetworkAccessPointState::StartAccessPoint:
        if (!shutdown)
        {
            WiFi.mode(WIFI_AP);
            WiFi.softAPConfig(this->accessPointIPAddress,
                              this->accessPointIPAddress,
                              this->accessPointSubnetmask);
            ret = WiFi.softAP(this->accesspointName,
                              "",
                              0,
                              0,
                              1);
            if (ret)
            {
                this->accessPointState = NetworkAccessPointState::SuperviseAccessPointConnection;
            }
        }
        else
        {
            this->accessPointState = NetworkAccessPointState::IdleAccessPoint;
        }
        break;

        // ================================ SuperviseAccessPointConnection ================================ //
    case NetworkAccessPointState::SuperviseAccessPointConnection:
        this->accessPointConnectedClients = WiFi.softAPgetStationNum();
        if (shutdown)
        {
            this->accessPointState = NetworkAccessPointState::ShutdownAccessPoint;
        }
        break;

        // ================================ ShutdownAccessPoint ================================ //
    case NetworkAccessPointState::ShutdownAccessPoint:
        ret = WiFi.softAPdisconnect(true);
        if (ret)
        {
            this->accessPointState = NetworkAccessPointState::IdleAccessPoint;
        }
        break;

        // ================================ IdleAccessPoint ================================ //
    case NetworkAccessPointState::IdleAccessPoint:
        if (!shutdown)
        {
            this->accessPointState = NetworkAccessPointState::StartAccessPoint;
        }
        break;

    default:
        Serial.println(F("Network unknown access point state!"));
        break;
    }
}

/**
 * @brief Handles the connection to the wifi network
 * 
 * @param shutdown If true the wifi handler will shutdown the connection change into the idle state
 */
void Network::HandleWiFi(bool shutdown)
{

    if (this->wifiState != this->memWifiState)
    {
        if (this->wifiState == NetworkWiFiState::SuperviseWiFiConnection)
        {
            NetworkWiFiInformation wiFiInformation = this->getWiFiInformation();
            this->information->FormatPrintNetworkWiFi(this->helper->BollToConnectionState(true),
                                                      wiFiInformation.hostname,
                                                      wiFiInformation.ipAddress,
                                                      wiFiInformation.subnetMask,
                                                      wiFiInformation.macAddress,
                                                      wiFiInformation.gatewayIpAddress);
        }
        if (this->wifiState == NetworkWiFiState::CheckWiFiDisconnect)
        {
            this->information->FormatPrintNetworkWiFi(this->helper->BollToConnectionState(false), "", "", "", "", "");
        }
        this->memWifiState = this->wifiState;
    }

    switch (this->wifiState)
    {
        // ================================ StartWifi ================================ //
    case NetworkWiFiState::StartWiFi:
        if (!shutdown)
        {
            if (this->filesystem->isConfigurationDataReady())
            {
                WiFi.mode(WIFI_STA);
                WiFi.hostname(this->filesystem->getConfigurationData().mqttClientName.c_str());
                WiFi.begin(this->filesystem->getConfigurationData().wifiSSID.c_str(),
                           this->filesystem->getConfigurationData().wifiPassword.c_str());
                this->wifiState = NetworkWiFiState::SuperviseWiFiConnection;
            }
        }
        else
        {
            this->wifiState = NetworkWiFiState::IdleWiFi;
        }
        break;

        // ================================ SuperviseWiFiConnection ================================ //
    case NetworkWiFiState::SuperviseWiFiConnection:
        if (WiFi.status() != WL_CONNECTED)
        {
            this->wifiState = NetworkWiFiState::CheckWiFiDisconnect;
            PrevMillis_WiFiTimeout = millis();
        }
        else
        {
            this->WiFiConnected = true;
        }
        if (shutdown)
        {
            this->wifiState = NetworkWiFiState::ShutdownWiFi;
        }
        break;

        // ================================ CheckWiFiDisconnect ================================ //
    case NetworkWiFiState::CheckWiFiDisconnect:
        if (WiFi.status() != WL_CONNECTED)
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_WiFiTimeout = millis();
            if (CurMillis_WiFiTimeout - PrevMillis_WiFiTimeout >= TimeOut_WiFiTimeout)
            {
                this->WiFiConnected = false;
                PrevMillis_WiFiTimeout = CurMillis_WiFiTimeout;
                WiFi.disconnect();
                this->wifiState = NetworkWiFiState::StartWiFi;
            }
        }
        else
        {
            this->wifiState = NetworkWiFiState::SuperviseWiFiConnection;
        }
        break;

        // ================================ ShutdownWiFi ================================ //
    case NetworkWiFiState::ShutdownWiFi:
        if (WiFi.status() == WL_CONNECTED)
        {
            WiFi.disconnect();
        }
        else
        {
            this->WiFiConnected = false;
            this->wifiState = NetworkWiFiState::IdleWiFi;
        }
        break;

        // ================================ IdleWiFi ================================ //
    case NetworkWiFiState::IdleWiFi:
        if (!shutdown)
        {
            this->wifiState = NetworkWiFiState::StartWiFi;
        }
        break;

    default:
        Serial.println(F("Network unknown wifi state!"));
        break;
    }
};

/**
 * Handels the MQTT connection after the wifi is connected
 * Subscribes to a list pre defined topics
 * Auto reconnects after dc and resubscribes to the defined topics
 */
void Network::HandleMqtt()
{

    if (this->mqttState != this->memMqttState)
    {
        if (this->mqttState == NetworkMQTTState::SuperviseMQTTConnection)
        {
            NetworkMQTTInformation mqttInformation = this->getMQTTInformation();
            this->information->FormatPrintNetworkMQTT(this->helper->BollToConnectionState(true),
                                                      String(mqttInformation.clientState),
                                                      mqttInformation.brokerIpAddress,
                                                      mqttInformation.brokerPort,
                                                      mqttInformation.clientName);
        }
        if (this->mqttState == NetworkMQTTState::CheckMQTTDisconnect)
        {
            this->information->FormatPrintNetworkMQTT(this->helper->BollToConnectionState(false), "", "", 0, "");
        }
        this->memMqttState = this->mqttState;
    }

    // Client state for this->helper print
    clientState = mqttClient.state();

    switch (this->mqttState)
    {
    case NetworkMQTTState::StartMQTT:
        // Only try reconnect when WiFi is connected
        if (this->WiFiConnected && this->filesystem->isConfigurationDataReady())
        {
            ConfigurationData data = this->filesystem->getConfigurationData();

            mqttClient.setClient(wifiMqtt);
            mqttClient.setServer(data.mqttBrokerIpAddress.c_str(),
                                 data.mqttBrokerPort);
            mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                                   { this->MqttCallback(topic, payload, length); });

            if (mqttClient.connect(data.mqttClientName.c_str(),
                                   data.mqttBrokerUsername.c_str(),
                                   data.mqttBrokerPassword.c_str()))
            {
                // ================ HomeAssistant ================ //
                /*
                    These Mqtt paths are for the paths given for the desired behavior when controlloing over homeassistant
                */
                // ==== Global ==== //
                // Sun
                mqttClient.subscribe("LEDController/Global/HomeAssistant/Sun/command");

                // Master
                mqttClient.subscribe("LEDController/Global/HomeAssistant/MasterPresent/command");

                // Alarm
                mqttClient.subscribe("LEDController/Global/HomeAssistant/Effect/Alarm/command");

                // Code Version
                // The installed code version of the LED Controller Mk4.1 is published under the following path on connect
                // "LEDController/" + data.mqttClientName + "/Version"

                // ==== Specific ==== //
                // Motion
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/command").c_str());

                // Strip 1
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/Power/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/Effect/command").c_str());

                // Strip 2
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/Power/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/Effect/command").c_str());

                // ==== Virtual ==== //
                // PIR
                /*
                    !!! Make sure that the retain flag is set to false for messages in this topic for this to work probably !!!
                    !!! You may need to clear all messages in the history of this topic with retain flag set to true !!!
                */
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Virtual/PIR/command").c_str());

                // ================ Json ================ //
                /*
                    These Mqtt paths are for the paths given for the desired behavior when controlloing over custom json data
                */
                // ==== Global ==== //
                mqttClient.subscribe("LEDController/Global/JSON/Sun/command");
                mqttClient.subscribe("LEDController/Global/JSON/MasterPresent/command");
                mqttClient.subscribe("LEDController/Global/JSON/Effect/Alarm/command");

                // ==== Specific ==== //
                // Motion
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/JSON/MotionDetection/command").c_str());

                // Strip 1
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/JSON/Strip1/command").c_str());

                // Strip 2
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/JSON/Strip2/command").c_str());

                // PIR
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/JSON/Virtual/PIR/command").c_str());

                // === Republish == //
                PublishLEDStripData();
                PublishMotionLEDStripData();
                PublishMotionDetected();
                PublishNetwork();
                PublishHeartbeat();
                PublishElectricalMeasurement();
                PublishCodeVersion();

                mqttState = NetworkMQTTState::SuperviseMQTTConnection;
            }
        }
        break;

    case NetworkMQTTState::SuperviseMQTTConnection:
        if (!mqttClient.connected())
        {
            mqttState = NetworkMQTTState::CheckMQTTDisconnect; // Check if dc occurred
            PrevMillis_MQTTTimeout = millis();                 // Set time for WiFi timeout check
        }
        else
        {
            this->MQTTConnected = true;
        }
        break;

    case NetworkMQTTState::CheckMQTTDisconnect:
        if (!mqttClient.connected())
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_MQTTTimeout = millis();
            if (CurMillis_MQTTTimeout - PrevMillis_MQTTTimeout >= TimeOut_MQTTTimeout)
            {
                this->MQTTConnected = false;
                PrevMillis_MQTTTimeout = CurMillis_MQTTTimeout;
                mqttClient.disconnect(); // Disconnect MQTT and start new connection
                mqttState = NetworkMQTTState::StartMQTT;
            }
        }
        else
        {
            mqttState = NetworkMQTTState::SuperviseMQTTConnection; // WiFi reconnected
        }
        break;

    default:
        Serial.println(F("Network unknown mqtt state!"));
        break;
    }
};

/**
 * Handles the Network Time Protocol for accurate time updates
 */
void Network::HandleNTP()
{

    switch (this->ntpState)
    {
        // ================================ StartNTP ================================ //
    case NetworkNTPState::StartNTP:
        this->ntpTimeClient.begin();
        this->ntpState = NetworkNTPState::SuperviseNTPConnection;
        break;

        // ================================ SuperviseNTPConnection ================================ //
    case NetworkNTPState::SuperviseNTPConnection:
        if (this->WiFiConnected)
        {
            // Get Time update
            unsigned long CurMillis_NTPTimeout = millis();
            if (CurMillis_NTPTimeout - this->PrevMillis_NTPTimeout >= this->TimeOut_NTPTimeout)
            {
                this->PrevMillis_NTPTimeout = CurMillis_NTPTimeout;
                bool updateSuccessful = this->ntpTimeClient.update();

                if (updateSuccessful)
                {
                    this->networkTimeData.hour = this->ntpTimeClient.getHours();
                    this->networkTimeData.minute = this->ntpTimeClient.getMinutes();
                    this->networkTimeData.second = this->ntpTimeClient.getSeconds();
                    this->networkTimeData.unix = this->ntpTimeClient.getEpochTime();

                    this->information->FormatPrintTime("Time",
                                                       String(this->networkTimeData.hour),
                                                       String(this->networkTimeData.minute),
                                                       String(this->networkTimeData.second),
                                                       String(this->networkTimeData.unix));
                }
            }
        }
        else
        {
            this->ntpState = NetworkNTPState::CheckNTPDisconnect;
        }
        break;

        // ================================ CheckNTPDisconnect ================================ //
    case NetworkNTPState::CheckNTPDisconnect:
        if (this->WiFiConnected)
        {
            this->PrevMillis_NTPTimeout = millis();
            this->ntpState = NetworkNTPState::SuperviseNTPConnection;
        }
        break;

    default:
        Serial.println(F("Network unknown ntp state!"));
        break;
    }
};

/**
 * @brief  MQTT callback function. Processes all the receives commands from the subscribed topics
 * 
 * @param topic A pointer to a char array containing the mqtt topic that calles this function with new data 
 * @param payload A pointer to a byte array with data send over the mqtt topic
 * @param length The length of the byte data array
 */
void Network::MqttCallback(char *topic, byte *payload, unsigned int length)
{

    char message[length + 1];
    char memMessage[length + 1];
    for (unsigned int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
        memMessage[i] = (char)payload[i];
    }
    message[length] = '\0';
    memMessage[length] = '\0';

    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // # ================================ HomeAssistant ================================ //
    /*
        Callback stuff for data received by home assistant endpoints
    */

    // ================ Global ================ //
    // ======== Sun ======== //
    if (String("LEDController/Global/HomeAssistant/Sun/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkData.sunUnderTheHorizon != (bool)data)
            {
                this->networkData.sunUnderTheHorizon = (bool)data;
                this->information->FormatPrintSingle("Sun under the horizon", String(this->networkData.sunUnderTheHorizon));
            }

            if ((bool)data)
            {
                this->detailedSunData.sunfallUnix = this->networkTimeData.unix;
                this->detailedSunData.isSunfallSet = true;
            }
            else
            {
                this->detailedSunData.sunriseUnix = this->networkTimeData.unix;
                this->detailedSunData.isSunriseSet = true;
            }
        }
    }
    // ======== MasterPresent ======== //
    else if (String("LEDController/Global/HomeAssistant/MasterPresent/command").equals(topic))
    {
        String temp_message = strtok(message, "\0");
        bool temp = temp_message.equals("home");
        if (this->networkData.masterPresent != temp)
        {
            this->networkData.masterPresent = temp;
            this->information->FormatPrintSingle("Master Present", String(this->networkData.masterPresent));
        }
    }
    // ======== Alarm ======== //
    else if (String("LEDController/Global/HomeAssistant/Effect/Alarm/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkData.alarm != (bool)data)
            {
                this->networkData.alarm = (bool)data;
                this->information->FormatPrintSingle("Alarm Active", String(this->networkData.alarm));
            }
        }
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkData.networkMotionData.timeBasedBrightnessChangeEnabled != (bool)data)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.timeBasedBrightnessChangeEnabled = (bool)data;
            }

            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkData.networkMotionData.motionDetectionEnabled != (bool)data)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.motionDetectionEnabled = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1000)
        {
            if (this->networkData.networkMotionData.timeout != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.timeout = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkData.networkMotionData.redColorValue != red ||
                this->networkData.networkMotionData.greenColorValue != green ||
                this->networkData.networkMotionData.blueColorValue != blue)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.redColorValue = red;
                this->networkData.networkMotionData.greenColorValue = green;
                this->networkData.networkMotionData.blueColorValue = blue;
            }

            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkData.networkMotionData.colorBrightnessValue = 0;
            }
            else
            {
                if (this->networkData.networkMotionData.colorBrightnessValue != data)
                {
                    this->motionDetectionDataPrint = true;
                    this->networkData.networkMotionData.colorBrightnessValue = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkData.networkMotionData.whiteTemperatureValue != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.whiteTemperatureValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkData.networkMotionData.whiteBrightnessValue != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkData.networkMotionData.whiteBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), memMessage);
        }
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkData.networkLEDStripData[0].power != (bool)data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkData.networkLEDStripData[0].power = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkData.networkLEDStripData[0].ledStripData.redColorValue != red ||
                this->networkData.networkLEDStripData[0].ledStripData.greenColorValue != green ||
                this->networkData.networkLEDStripData[0].ledStripData.blueColorValue != blue)
            {
                this->ledStripDataPrint[0] = true;
                this->networkData.networkLEDStripData[0].ledStripData.redColorValue = red;
                this->networkData.networkLEDStripData[0].ledStripData.greenColorValue = green;
                this->networkData.networkLEDStripData[0].ledStripData.blueColorValue = blue;
            }

            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkData.networkLEDStripData[0].ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                if (this->networkData.networkLEDStripData[0].ledStripData.colorBrightnessValue != data)
                {
                    this->ledStripDataPrint[0] = true;
                    this->networkData.networkLEDStripData[0].ledStripData.colorBrightnessValue = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkData.networkLEDStripData[0].ledStripData.whiteTemperatureValue != data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkData.networkLEDStripData[0].ledStripData.whiteTemperatureValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkData.networkLEDStripData[0].ledStripData.whiteBrightnessValue != data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkData.networkLEDStripData[0].ledStripData.whiteBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Effect/command").equals(topic))
    {
        if (this->networkData.networkLEDStripData[0].effect != this->helper->StringToSingleLEDEffect(message))
        {
            this->ledStripDataPrint[0] = true;
            this->networkData.networkLEDStripData[0].effect = this->helper->StringToSingleLEDEffect(message);
        }
        mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), memMessage);
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 2)
        {
            if (this->networkData.networkLEDStripData[1].power != (bool)data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkData.networkLEDStripData[1].power = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkData.networkLEDStripData[1].ledStripData.redColorValue != red ||
                this->networkData.networkLEDStripData[1].ledStripData.greenColorValue != green ||
                this->networkData.networkLEDStripData[1].ledStripData.blueColorValue != blue)
            {
                this->ledStripDataPrint[1] = true;
                this->networkData.networkLEDStripData[1].ledStripData.redColorValue = red;
                this->networkData.networkLEDStripData[1].ledStripData.greenColorValue = green;
                this->networkData.networkLEDStripData[1].ledStripData.blueColorValue = blue;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkData.networkLEDStripData[1].ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                if (this->networkData.networkLEDStripData[1].ledStripData.colorBrightnessValue != data)
                {
                    this->ledStripDataPrint[1] = true;
                    this->networkData.networkLEDStripData[1].ledStripData.colorBrightnessValue = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkData.networkLEDStripData[1].ledStripData.whiteTemperatureValue != data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkData.networkLEDStripData[1].ledStripData.whiteTemperatureValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkData.networkLEDStripData[1].ledStripData.whiteBrightnessValue != data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkData.networkLEDStripData[1].ledStripData.whiteBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Effect/command").equals(topic))
    {
        if (this->networkData.networkLEDStripData[1].effect != this->helper->StringToSingleLEDEffect(message))
        {
            this->ledStripDataPrint[1] = true;
            this->networkData.networkLEDStripData[1].effect = this->helper->StringToSingleLEDEffect(message);
        }

        mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), memMessage);
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Virtual/PIR/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data == 1)
        {
            this->networkData.virtualPIRSensorTriggered = true;
        }
        this->information->FormatPrintSingle("Virtual PIR Sensor", String(this->networkData.virtualPIRSensorTriggered));
    }

    // # ================================ JSON ================================ //
    /*
        Callback stuff for data received by json endpoints
    */

    // ================ Global ================ //
    // ======== Sun ======== //
    else if (String("LEDController/Global/JSON/Sun/command").equals(topic))
    {
    }
    // ======== MasterPresent ======== //
    else if (String("LEDController/Global/JSON/MasterPresent/command").equals(topic))
    {
    }

    // ======== Effects ======== //
    // ======== Alarm ======== //
    else if (String("LEDController/Global/JSON/Effect/Alarm/command").equals(topic))
    {
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/JSON/MotionDetection/command").equals(topic))
    {
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/JSON/Strip1/command").equals(topic))
    {
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/JSON/Strip2/command").equals(topic))
    {
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configurationData.mqttClientName + "/JSON/Virtual/PIR/command").equals(topic))
    {
    }
}

void Network::HandleRepublish()
{
    // For now disabled

    unsigned long curMillis = millis();

    // == Motion Detection
    if (curMillis - prevMillisPublishMotionDetected >= timeoutPublishMotionDetected)
    {
        PublishMotionDetected();
    }

    // == LED Strip Data
    if (curMillis - prevMillisPublishLEDStripData >= timeoutPublishLEDStripData)
    {
        PublishLEDStripData();
    }

    // == Electrical Messurement
    if (curMillis - prevMillisPublishElectricalMeasurement >= timeoutPublishElectricalMeasurement)
    {
        PublishElectricalMeasurement();
    }

    // == Heartbeat
    if (curMillis - prevMillisPublishHeartbeat >= timeoutPublishHeartbeat)
    {
        PublishHeartbeat();
    }

    // == LED Strip Motion Data
    if (curMillis - prevMillisPublishMotionLEDStripData >= timeoutPublishMotionLEDStripData)
    {
        PublishMotionLEDStripData();
    }

    // == Network
    if (curMillis - prevMillisPublishNetwork >= timeoutPublishNetwork)
    {
        PublishNetwork();
    }
}

/**
 * @brief Publishes a update of the motion detection data over mqtt
 * 
 */
void Network::PublishMotionDetected()
{
    prevMillisPublishMotionDetected = millis();

    String message = "";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(this->pirReader->getPIRReaderData().motionDetected);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Motion/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensorTriggered);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Motion/PIR/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensor1Triggered);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor1/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensor2Triggered);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor2/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().virtualSensorTriggered);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Motion/PIR/VirtualSensor/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes a update of the led strip data over mqtt
 * 
 */
void Network::PublishLEDStripData()
{
    prevMillisPublishLEDStripData = millis();

    String message = "";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(this->getNetworkLEDStripData(1).power);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(1).ledStripData.redColorValue + String(",") + this->getNetworkLEDStripData(1).ledStripData.greenColorValue + String(",") + this->getNetworkLEDStripData(1).ledStripData.blueColorValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(1).ledStripData.colorBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(1).ledStripData.whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(1).ledStripData.whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), message.c_str());
    message = String(this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(1).effect));
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), message.c_str());

    message = String(this->getNetworkLEDStripData(2).power);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(2).ledStripData.redColorValue + String(",") + this->getNetworkLEDStripData(2).ledStripData.greenColorValue + String(",") + this->getNetworkLEDStripData(2).ledStripData.blueColorValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(2).ledStripData.colorBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(2).ledStripData.whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), message.c_str());
    message = String(this->getNetworkLEDStripData(2).ledStripData.whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), message.c_str());
    message = String(this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(2).effect));
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes a update of the electrical measurement data over mqtt
 * 
 */
void Network::PublishElectricalMeasurement()
{
    prevMillisPublishElectricalMeasurement = millis();

    String message = "";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(powerMeasurement->valuePower_mW);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/ElectricalMesurement/CurrentPower/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueBus_V);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/ElectricalMesurement/BusVoltage/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueCurrent_mA);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/ElectricalMesurement/CurrentAmpere/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes a heartbear over mqtt
 * 
 */
void Network::PublishHeartbeat()
{
    prevMillisPublishHeartbeat = millis();

    String message = "pulse";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Heartbeat/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes a update of the motion led strip data over mqtt
 * 
 */
void Network::PublishMotionLEDStripData()
{
    prevMillisPublishMotionLEDStripData = millis();

    String message = "";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(this->getNetworkMotionData().motionDetectionEnabled);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/Enable/state").c_str(), message.c_str());
    message = String(this->getNetworkMotionData().redColorValue + String(",") + this->getNetworkMotionData().greenColorValue + String(",") + this->getNetworkMotionData().blueColorValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), message.c_str());
    message = String(this->getNetworkMotionData().colorBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), message.c_str());
    message = String(this->getNetworkMotionData().whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), message.c_str());
    message = String(this->getNetworkMotionData().whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes a update of the network information over mqtt
 * 
 */
void Network::PublishNetwork()
{
    prevMillisPublishNetwork = millis();

    String message = "";
    ConfigurationData configurationData = this->filesystem->getConfigurationData();
    NetworkWiFiInformation wiFiInformation = this->getWiFiInformation();

    // ================================================ HOMEASSISTANT ================================================ //
    message = wiFiInformation.ipAddress;
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Network/IPAddress/state").c_str(), message.c_str());
    message = wiFiInformation.macAddress;
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/HomeAssistant/Network/MACAddress/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes the current code version over mqtt
 * 
 */
void Network::PublishCodeVersion()
{
    ConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    mqttClient.publish(("LEDController/" + configurationData.mqttClientName + "/Version").c_str(), codeVersion.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Request for the network componente to change into access point mode
 * 
 */
void Network::RequestChangeToAccessPointMode()
{
    if (this->wiFiMode && !this->accessPointMode)
    {
        this->changeToAccessPointModeRequest = true;
    }
}

/**
 * @brief Request for the network componente to change into wifi mode
 * 
 */
void Network::RequestChangeToWiFiMode()
{
    if (this->accessPointMode && !this->wiFiMode)
    {
        this->changeToWiFiModeRequest = true;
    }
}

/**
 * @return The current WiFi this->helper based on the used WiFi mode (AP or STA) 
 */
NetworkWiFiInformation Network::getWiFiInformation()
{
    NetworkWiFiInformation wiFiInformation = {};

    wiFiInformation.ipAddress = WiFi.localIP().toString().c_str();
    wiFiInformation.subnetMask = WiFi.subnetMask().toString().c_str();
    wiFiInformation.macAddress = WiFi.macAddress();
    wiFiInformation.gatewayIpAddress = WiFi.gatewayIP().toString().c_str();
    wiFiInformation.hostname = WiFi.hostname();
    wiFiInformation.inWiFiMode = this->wiFiMode;
    wiFiInformation.inAccessPointMode = this->accessPointMode;
    wiFiInformation.isWiFiConnected = this->WiFiConnected;

    return wiFiInformation;
}

/**
 * 
 * @return The current MQTT this->helper 
 */
NetworkMQTTInformation Network::getMQTTInformation()
{
    NetworkMQTTInformation mqttInformation = {};

    mqttInformation.clientName = this->filesystem->getConfigurationData().mqttClientName.c_str();
    mqttInformation.brokerIpAddress = this->filesystem->getConfigurationData().mqttBrokerIpAddress.c_str();
    mqttInformation.brokerPort = this->filesystem->getConfigurationData().mqttBrokerPort;
    mqttInformation.isMQTTConnected = this->MQTTConnected;
    mqttInformation.clientState = this->clientState;

    return mqttInformation;
}

bool Network::isWiFiConnected()
{
    return this->WiFiConnected;
}

bool Network::isMQTTConnected()
{
    return this->MQTTConnected;
}

bool Network::isInWiFiMode()
{
    return this->wiFiMode;
}

bool Network::isInAccessPointMode()
{
    return this->accessPointMode;
}

bool Network::isVirtualPIRSensorTriggered()
{
    return this->networkData.virtualPIRSensorTriggered;
}

void Network::resetVirtualPIRSensor()
{
    this->networkData.virtualPIRSensorTriggered = false;
}

bool Network::isMasterPresent()
{
    return this->networkData.masterPresent;
}

bool Network::isAlarm()
{
    return this->networkData.alarm;
}

bool Network::isSunUnderTheHorizon()
{
    return this->networkData.sunUnderTheHorizon;
}

NetworkMotionData Network::getNetworkMotionData()
{
    return this->networkData.networkMotionData;
}

void Network::UpdateNetworkMotionData(NetworkMotionData data)
{
    this->networkData.networkMotionData = data;
    this->PublishMotionLEDStripData();
}

/**
 * @brief Returns the network led strip data of the given strip
 * 
 * @param stripID The strip ID form which we want the data
 * @return NetworkLEDStripData 
 */
NetworkLEDStripData Network::getNetworkLEDStripData(uint8_t stripID)
{
    // Strip ID's are 1 and 2 but the index of the array starts at 0
    stripID--;
    NetworkLEDStripData data = {};
    if (stripID >= 0 && stripID <= sizeof(this->networkData.networkLEDStripData))
    {
        data = this->networkData.networkLEDStripData[stripID];
    }

    return data;
}

void Network::UpdateNetworkLEDStripData(uint8_t stripID, NetworkLEDStripData data)
{
    // Strip ID's are 1 and 2 but the index of the array starts at 0
    stripID--;
    if (stripID >= 0 && stripID <= sizeof(this->networkData.networkLEDStripData))
    {
        this->networkData.networkLEDStripData[stripID] = data;
        this->PublishLEDStripData();
    }
}

NetworkData Network::getNetworkData()
{
    return this->networkData;
}

DetailedSunData Network::getDetailedSunData()
{
    return this->detailedSunData;
}

NetworkTimeData Network::getNetworkTimeData()
{
    return this->networkTimeData;
}
