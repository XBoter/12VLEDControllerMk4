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

        this->networkMotionData.VirtualPIRSensorTriggered = false;

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
        // We use here the same flag for our parameter update
        this->parameterhandler->updateMotionParameter(this->getNetworkMotionData());

        this->motionDetectionDataPrint = false;
        this->information->FormatPrintMotion("Motion Parameter",
                                             this->helper->BoolToString(this->getNetworkMotionData().MotionDetectionEnabled),
                                             this->helper->BoolToString(this->getNetworkMotionData().TimeBasedBrightnessChangeEnabled),
                                             String(this->getNetworkMotionData().MotionDetectionTimeout),
                                             String(this->getNetworkMotionData().Red),
                                             String(this->getNetworkMotionData().Green),
                                             String(this->getNetworkMotionData().Blue),
                                             String(this->getNetworkMotionData().ColorBrightness),
                                             String(this->getNetworkMotionData().WhiteTemperature),
                                             String(this->getNetworkMotionData().WhiteTemperatureBrightness));
    }
    if (this->ledStripDataPrint[0])
    {
        // We use here the same flag for our parameter update
        this->parameterhandler->updateLEDStripParameter(0, this->getNetworkLEDStripData(1));

        this->ledStripDataPrint[0] = false;
        this->information->FormatPrintLEDStrip("LED Strip 1",
                                               this->helper->BoolToString(this->getNetworkLEDStripData(1).Power),
                                               String(this->getNetworkLEDStripData(1).Red),
                                               String(this->getNetworkLEDStripData(1).Green),
                                               String(this->getNetworkLEDStripData(1).Blue),
                                               String(this->getNetworkLEDStripData(1).ColorFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ColorFadeCurve),
                                               String(this->getNetworkLEDStripData(1).ColorBrightness),
                                               String(this->getNetworkLEDStripData(1).ColorBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).ColorBrightnessFadeCurve),
                                               String(this->getNetworkLEDStripData(1).WhiteTemperature),
                                               String(this->getNetworkLEDStripData(1).WhiteTemperatureFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).WhiteTemperatureFadeCurve),
                                               String(this->getNetworkLEDStripData(1).WhiteTemperatureBrightness),
                                               String(this->getNetworkLEDStripData(1).WhiteTemperatureBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(1).WhiteTemperatureBrightnessFadeCurve),
                                               this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(1).Effect));
    }
    if (this->ledStripDataPrint[1])
    {
        // We use here the same flag for our parameter update
        this->parameterhandler->updateLEDStripParameter(1, this->getNetworkLEDStripData(2));

        this->ledStripDataPrint[1] = false;
        this->information->FormatPrintLEDStrip("LED Strip 2",
                                               this->helper->BoolToString(this->getNetworkLEDStripData(2).Power),
                                               String(this->getNetworkLEDStripData(2).Red),
                                               String(this->getNetworkLEDStripData(2).Green),
                                               String(this->getNetworkLEDStripData(2).Blue),
                                               String(this->getNetworkLEDStripData(2).ColorFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ColorFadeCurve),
                                               String(this->getNetworkLEDStripData(2).ColorBrightness),
                                               String(this->getNetworkLEDStripData(2).ColorBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).ColorBrightnessFadeCurve),
                                               String(this->getNetworkLEDStripData(2).WhiteTemperature),
                                               String(this->getNetworkLEDStripData(2).WhiteTemperatureFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).WhiteTemperatureFadeCurve),
                                               String(this->getNetworkLEDStripData(2).WhiteTemperatureBrightness),
                                               String(this->getNetworkLEDStripData(2).WhiteTemperatureBrightnessFadeTime),
                                               this->helper->FadeCurveToString(this->getNetworkLEDStripData(2).WhiteTemperatureBrightnessFadeCurve),
                                               this->helper->SingleLEDEffectToString(this->getNetworkLEDStripData(2).Effect));
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
                this->accessPointReady = true;
                this->helper->SimplePrint(F("Network changing to Access Point mode"));
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
            this->helper->SimplePrint(F("Network shut down Access Point mode"));
            this->accessPointReady = false;
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
                WiFi.hostname(this->filesystem->getConfigurationData().MQTTClientName.c_str());
                WiFi.begin(this->filesystem->getConfigurationData().WiFiSSID.c_str(),
                           this->filesystem->getConfigurationData().WiFiPassword);

                this->helper->SimplePrint(F("Network changing to WiFi mode"));

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
            this->helper->SimplePrint(F("Network shut down WiFi mode"));
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
            FilesystemConfigurationData data = this->filesystem->getConfigurationData();

            mqttClient.setClient(wifiMqtt);
            mqttClient.setServer(data.MQTTBrokerIpAddress.c_str(),
                                 data.MQTTBrokerPort);
            mqttClient.setCallback(std::bind(&Network::MqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

            if (mqttClient.connect(data.MQTTClientName.c_str(),
                                   data.MQTTBrokerUsername.c_str(),
                                   data.MQTTBrokerPassword.c_str()))
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
                // "LEDController/" + data.MQTTClientName + "/Version"

                // ==== Specific ==== //
                // Motion
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/Enabled/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/MotionDetection/Timeout/command").c_str());

                // Strip 1
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/Power/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip1/Effect/command").c_str());

                // Strip 2
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/Power/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/RGB/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/White/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/White/Brightness/command").c_str());
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Strip2/Effect/command").c_str());

                // ==== Virtual ==== //
                // PIR
                /*
                    !!! Make sure that the retain flag is set to false for messages in this topic for this to work probably !!!
                    !!! You may need to clear all messages in the history of this topic with retain flag set to true !!!
                */
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/HomeAssistant/Virtual/PIR/command").c_str());

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
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/JSON/MotionDetection/command").c_str());

                // Strip 1
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/JSON/Strip1/command").c_str());

                // Strip 2
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/JSON/Strip2/command").c_str());

                // PIR
                mqttClient.subscribe(("LEDController/" + data.MQTTClientName + "/JSON/Virtual/PIR/command").c_str());

                // === Republish == //
                this->UpdateNetworkLEDStripData(0, this->networkLEDStripData[0], true);
                this->UpdateNetworkLEDStripData(1, this->networkLEDStripData[1], true);
                this->UpdateNetworkMotionData(this->networkMotionData, true);
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

    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();

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
            if (this->networkMotionData.SunUnderTheHorizon != (bool)data)
            {
                this->networkMotionData.SunUnderTheHorizon = (bool)data;
                this->information->FormatPrintSingle("Sun under the horizon", String(this->networkMotionData.SunUnderTheHorizon));
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
        if (this->networkLEDStripData[0].MasterPresent != temp)
        {
            this->networkLEDStripData[0].MasterPresent = temp;
            this->networkLEDStripData[1].MasterPresent = temp;
            this->information->FormatPrintSingle("Master Present", String(this->networkLEDStripData[0].MasterPresent));
        }
    }
    // ======== Alarm ======== //
    else if (String("LEDController/Global/HomeAssistant/Effect/Alarm/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkLEDStripData[0].AlarmActive != (bool)data)
            {
                this->networkLEDStripData[0].AlarmActive = (bool)data;
                this->networkLEDStripData[1].AlarmActive = (bool)data;
                this->information->FormatPrintSingle("Alarm Active", String(this->networkLEDStripData[0].AlarmActive));
            }
        }
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkMotionData.TimeBasedBrightnessChangeEnabled != (bool)data)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.TimeBasedBrightnessChangeEnabled = (bool)data;
            }

            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/Enabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkMotionData.MotionDetectionEnabled != (bool)data)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.MotionDetectionEnabled = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/Enabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/Timeout/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1000)
        {
            if (this->networkMotionData.MotionDetectionTimeout != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.MotionDetectionTimeout = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/Timeout/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkMotionData.Red != red ||
                this->networkMotionData.Green != green ||
                this->networkMotionData.Blue != blue)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.Red = red;
                this->networkMotionData.Green = green;
                this->networkMotionData.Blue = blue;
            }

            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkMotionData.ColorBrightness = 0;
            }
            else
            {
                if (this->networkMotionData.ColorBrightness != data)
                {
                    this->motionDetectionDataPrint = true;
                    this->networkMotionData.ColorBrightness = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkMotionData.WhiteTemperature != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.WhiteTemperature = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkMotionData.WhiteTemperatureBrightness != data)
            {
                this->motionDetectionDataPrint = true;
                this->networkMotionData.WhiteTemperatureBrightness = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), memMessage);
        }
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            if (this->networkLEDStripData[0].Power != (bool)data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkLEDStripData[0].Power = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkLEDStripData[0].Red != red ||
                this->networkLEDStripData[0].Green != green ||
                this->networkLEDStripData[0].Blue != blue)
            {
                this->ledStripDataPrint[0] = true;
                this->networkLEDStripData[0].Red = red;
                this->networkLEDStripData[0].Green = green;
                this->networkLEDStripData[0].Blue = blue;
            }

            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkLEDStripData[0].ColorBrightness = 0;
            }
            else
            {
                if (this->networkLEDStripData[0].ColorBrightness != data)
                {
                    this->ledStripDataPrint[0] = true;
                    this->networkLEDStripData[0].ColorBrightness = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkLEDStripData[0].WhiteTemperature != data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkLEDStripData[0].WhiteTemperature = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkLEDStripData[0].WhiteTemperatureBrightness != data)
            {
                this->ledStripDataPrint[0] = true;
                this->networkLEDStripData[0].WhiteTemperatureBrightness = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Effect/command").equals(topic))
    {
        if (this->networkLEDStripData[0].Effect != this->helper->StringToSingleLEDEffect(message))
        {
            this->ledStripDataPrint[0] = true;
            this->networkLEDStripData[0].Effect = this->helper->StringToSingleLEDEffect(message);
        }
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), memMessage);
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 2)
        {
            if (this->networkLEDStripData[1].Power != (bool)data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkLEDStripData[1].Power = (bool)data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            if (this->networkLEDStripData[1].Red != red ||
                this->networkLEDStripData[1].Green != green ||
                this->networkLEDStripData[1].Blue != blue)
            {
                this->ledStripDataPrint[1] = true;
                this->networkLEDStripData[1].Red = red;
                this->networkLEDStripData[1].Green = green;
                this->networkLEDStripData[1].Blue = blue;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                this->networkLEDStripData[1].ColorBrightness = 0;
            }
            else
            {
                if (this->networkLEDStripData[1].ColorBrightness != data)
                {
                    this->ledStripDataPrint[1] = true;
                    this->networkLEDStripData[1].ColorBrightness = data;
                }
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            if (this->networkLEDStripData[1].WhiteTemperature != data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkLEDStripData[1].WhiteTemperature = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            if (this->networkLEDStripData[1].WhiteTemperatureBrightness != data)
            {
                this->ledStripDataPrint[1] = true;
                this->networkLEDStripData[1].WhiteTemperatureBrightness = data;
            }
            mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Effect/command").equals(topic))
    {
        if (this->networkLEDStripData[1].Effect != this->helper->StringToSingleLEDEffect(message))
        {
            this->ledStripDataPrint[1] = true;
            this->networkLEDStripData[1].Effect = this->helper->StringToSingleLEDEffect(message);
        }

        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), memMessage);
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Virtual/PIR/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data == 1)
        {
            this->networkMotionData.VirtualPIRSensorTriggered = true;
        }
        this->information->FormatPrintSingle("Virtual PIR Sensor", String(this->networkMotionData.VirtualPIRSensorTriggered));
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
    else if (String("LEDController/" + configurationData.MQTTClientName + "/JSON/MotionDetection/command").equals(topic))
    {
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/JSON/Strip1/command").equals(topic))
    {
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/JSON/Strip2/command").equals(topic))
    {
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configurationData.MQTTClientName + "/JSON/Virtual/PIR/command").equals(topic))
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
    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(this->pirReader->getPIRReaderData().motionDetected);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Motion/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensorTriggered);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Motion/PIR/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensor1Triggered);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Motion/PIR/Sensor1/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().sensor2Triggered);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Motion/PIR/Sensor2/state").c_str(), message.c_str());
    message = String(this->pirReader->getPIRReaderData().virtualSensorTriggered);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Motion/PIR/VirtualSensor/state").c_str(), message.c_str());

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
    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    message = String(powerMeasurement->valuePower_mW);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/ElectricalMesurement/CurrentPower/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueBus_V);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/ElectricalMesurement/BusVoltage/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueCurrent_mA);
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/ElectricalMesurement/CurrentAmpere/state").c_str(), message.c_str());

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
    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Heartbeat/state").c_str(), message.c_str());

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
    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();
    NetworkWiFiInformation wiFiInformation = this->getWiFiInformation();

    // ================================================ HOMEASSISTANT ================================================ //
    message = wiFiInformation.ipAddress;
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Network/IPAddress/state").c_str(), message.c_str());
    message = wiFiInformation.macAddress;
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Network/MACAddress/state").c_str(), message.c_str());

    // ================================================ JSON ================================================ //
}

/**
 * @brief Publishes the current code version over mqtt
 * 
 */
void Network::PublishCodeVersion()
{
    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();

    // ================================================ HOMEASSISTANT ================================================ //
    mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/Version").c_str(), codeVersion.c_str());

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

    mqttInformation.clientName = this->filesystem->getConfigurationData().MQTTClientName.c_str();
    mqttInformation.brokerIpAddress = this->filesystem->getConfigurationData().MQTTBrokerIpAddress.c_str();
    mqttInformation.brokerPort = this->filesystem->getConfigurationData().MQTTBrokerPort;
    mqttInformation.isMQTTConnected = this->MQTTConnected;
    mqttInformation.clientState = this->clientState;

    return mqttInformation;
}

bool Network::isWiFiConnected()
{
    return this->WiFiConnected;
}

bool Network::isAccessPointReady()
{
    return this->accessPointReady;
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
    return this->networkMotionData.VirtualPIRSensorTriggered;
}

void Network::resetVirtualPIRSensor()
{
    this->networkMotionData.VirtualPIRSensorTriggered = false;
}

bool Network::isMasterPresent(uint8_t stripID)
{
    if (stripID >= 0 && stripID < sizeof(this->networkLEDStripData))
    {
        return this->networkLEDStripData[stripID].MasterPresent;
    }
}

bool Network::isAlarm(uint8_t stripID)
{
    if (stripID >= 0 && stripID < sizeof(this->networkLEDStripData))
    {
        return this->networkLEDStripData[stripID].AlarmActive;
    }
}

bool Network::isSunUnderTheHorizon()
{
    return this->networkMotionData.SunUnderTheHorizon;
}

NetworkMotionData Network::getNetworkMotionData()
{
    return this->networkMotionData;
}

void Network::UpdateNetworkMotionData(NetworkMotionData data, bool republish)
{

    FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();
    String message = "";

    if (this->networkMotionData.MotionDetectionEnabled != data.MotionDetectionEnabled || republish)
    {
        message = String(data.MotionDetectionEnabled);
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/Enable/state").c_str(), message.c_str());
    }
    if (this->networkMotionData.Red != data.Red ||
        this->networkMotionData.Green != data.Green ||
        this->networkMotionData.Blue != data.Blue ||
        republish)
    {
        message = String(data.Red + String(",") + data.Green + String(",") + data.Blue);
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), message.c_str());
    }
    if (this->networkMotionData.ColorBrightness != data.ColorBrightness || republish)
    {
        message = String(data.ColorBrightness);
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), message.c_str());
    }
    if (this->networkMotionData.WhiteTemperature != data.WhiteTemperature || republish)
    {
        message = String(data.WhiteTemperature);
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), message.c_str());
    }
    if (this->networkMotionData.WhiteTemperatureBrightness != data.WhiteTemperatureBrightness || republish)
    {
        message = String(data.WhiteTemperatureBrightness);
        mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), message.c_str());
    }

    this->networkMotionData = data;
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
    if (stripID >= 0 && stripID <= sizeof(this->networkLEDStripData))
    {
        data = this->networkLEDStripData[stripID];
    }

    return data;
}

void Network::UpdateNetworkLEDStripData(uint8_t stripID, NetworkLEDStripData data, bool republish)
{
    // Strip ID's are 1 and 2 but the index of the array starts at 0
    stripID--;
    if (stripID >= 0 && stripID <= sizeof(this->networkLEDStripData))
    {
        FilesystemConfigurationData configurationData = this->filesystem->getConfigurationData();
        String message = "";

        if (stripID == 1)
        {
            if (this->networkLEDStripData[stripID].Power != data.Power || republish)
            {
                message = String(data.Power);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Power/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].Red != data.Red ||
                this->networkLEDStripData[stripID].Green != data.Green ||
                this->networkLEDStripData[stripID].Blue != data.Blue ||
                republish)
            {
                message = String(data.Red + String(",") + data.Green + String(",") + data.Blue);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].ColorBrightness != data.ColorBrightness || republish)
            {
                message = String(data.ColorBrightness);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].WhiteTemperature != data.WhiteTemperature || republish)
            {
                message = String(data.WhiteTemperature);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].WhiteTemperatureBrightness != data.WhiteTemperatureBrightness || republish)
            {
                message = String(data.WhiteTemperatureBrightness);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].Effect != data.Effect || republish)
            {
                message = String(this->helper->SingleLEDEffectToString(data.Effect));
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), message.c_str());
            }
        }

        if (stripID == 2)
        {
            if (this->networkLEDStripData[stripID].Power != data.Power || republish)
            {
                message = String(data.Power);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Power/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].Red != data.Red ||
                this->networkLEDStripData[stripID].Green != data.Green ||
                this->networkLEDStripData[stripID].Blue != data.Blue ||
                republish)
            {
                message = String(data.Red + String(",") + data.Green + String(",") + data.Blue);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].ColorBrightness != data.ColorBrightness || republish)
            {
                message = String(data.ColorBrightness);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].WhiteTemperature != data.WhiteTemperature || republish)
            {
                message = String(data.WhiteTemperature);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].WhiteTemperatureBrightness != data.WhiteTemperatureBrightness || republish)
            {
                message = String(data.WhiteTemperatureBrightness);
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), message.c_str());
            }
            if (this->networkLEDStripData[stripID].Effect != data.Effect || republish)
            {
                message = String(this->helper->SingleLEDEffectToString(data.Effect));
                mqttClient.publish(("LEDController/" + configurationData.MQTTClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), message.c_str());
            }
        }

        this->networkLEDStripData[stripID] = data;
    }
}

DetailedSunData Network::getDetailedSunData()
{
    return this->detailedSunData;
}

NetworkTimeData Network::getNetworkTimeData()
{
    return this->networkTimeData;
}
