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
                           Information *information,
                           PirReader *pirReader,
                           PowerMeasurement *powerMeasurement)
{
    this->filesystem = filesystem;
    this->information = information;
    this->pirReader = pirReader;
    this->powerMeasurement = powerMeasurement;
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
        data = configuration->getData();

        mqttClient.setClient(wifiMqtt);
        mqttClient.setServer(data.mqttBrokerIpAddress.c_str(),
                             data.mqttBrokerPort);
        mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                               { this->MqttCallback(topic, payload, length); });

        timeClient.begin();

        wifiState = NetworkWiFiState::StartWifi;
        mqttState = NetworkMQTTState::StartMqtt;

        virtualPIRSensorTriggered = false;

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
            this->isInWiFiMode = true;
            this->isInAccessPointMode = false;
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
            this->isInWiFiMode = false;
            this->isInAccessPointMode = true;
            this->shutdownAccessPoint = false;
            this->changeToAccessPointModeRequest = false;
        }
    }
    HandleAccessPoint(shutdown = this->shutdownAccessPoint);
    HandleWiFi(shutdown = this->shutdownWiFi);

    // ==== MQTT ==== //
    mqttClient.loop();
    HandleMqtt();

    // ==== NTP ==== //
    HandleNTP();

    // ==== REPUBLISH ==== //
    HandleRepublish();
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
            WiFi.softAPConfig(local_ip = this->accessPointIPAddress,
                              gateway = this->accessPointIPAddress,
                              subnet = this->accessPointSubnetmask);
            ret = WiFi.softAP(ssid = this->accesspointName,
                              psk = "",
                              channel = 0,
                              ssid_hidden = 0,
                              max_connection = 1);
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
        ret = WiFi.softAPdisconnect(wifioff = true);
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
    switch (this->wifiState)
    {
        // ================================ StartWifi ================================ //
    case NetworkWiFiState::StartWifi:
        if (!shutdown)
        {
            if (this->filesystem->isConfigurationDataReady())
            {
                WiFi.mode(WIFI_STA);
                WiFi.hostname(this->filesystem.getConfigurationData().mqttClientName.c_str());
                WiFi.begin(this->filesystem.getConfigurationData().wifiSSID.c_str(),
                           this->filesystem.getConfigurationData().wifiPassword.c_str());
                this->wifiState = NetworkWiFiState::SuperviseWiFiConnection;
            }
        }
        else
        {
            this->wifiState = NetworkWiFiState::Idle;
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
            this->isWiFiConnected = true;
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
                this->isWiFiConnected = false;
                PrevMillis_WiFiTimeout = CurMillis_WiFiTimeout;
                WiFi.disconnect();
                this->wifiState = NetworkWiFiState::StartWifi;
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
            this->isWiFiConnected = false;
            this->wifiState = NetworkWiFiState::IdleWiFi;
        }
        break;

        // ================================ IdleWiFi ================================ //
    case NetworkWiFiState::IdleWiFi:
        if (!shutdown)
        {
            this->wifiState = NetworkWiFiState::StartWifi;
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

    // Client state for information print
    clientState = mqttClient.state();

    switch (mqttState)
    {
    case NetworkMQTTState::StartMqtt:
        // Only try reconnect when WiFi is connected
        if (isWiFiConnected)
        {
            if (mqttClient.connect(this->filesystem.getConfigurationData().mqttClientName.c_str(),
                                   this->filesystem.getConfigurationData().mqttBrokerUsername.c_str(),
                                   this->filesystem.getConfigurationData().mqttBrokerPassword.c_str()))
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

                mqttState = NetworkMQTTState::SuperviseMqttConnection;
            }
        }
        break;

    case NetworkMQTTState::SuperviseMqttConnection:
        if (!mqttClient.connected())
        {
            mqttState = NetworkMQTTState::CheckMqttDisconnect; // Check if dc occurred
            PrevMillis_MQTTTimeout = millis();                 // Set time for WiFi timeout check
        }
        else
        {
            mqttConnected = true;
        }
        break;

    case NetworkMQTTState::CheckMqttDisconnect:
        if (!mqttClient.connected())
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_MQTTTimeout = millis();
            if (CurMillis_MQTTTimeout - PrevMillis_MQTTTimeout >= TimeOut_MQTTTimeout)
            {
                mqttConnected = false;
                PrevMillis_MQTTTimeout = CurMillis_MQTTTimeout;
                mqttClient.disconnect(); // Disconnect MQTT and start new connection
                mqttState = NetworkMQTTState::StartMqtt;
            }
        }
        else
        {
            mqttState = NetworkMQTTState::SuperviseMqttConnection; // WiFi reconnected
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
    // Get Time update
    unsigned long CurMillis_NTPTimeout = millis();
    if (CurMillis_NTPTimeout - PrevMillis_NTPTimeout >= TimeOut_NTPTimeout)
    {
        PrevMillis_NTPTimeout = CurMillis_NTPTimeout;
        bool updateSuccessful = timeClient.update();

        if (updateSuccessful)
        {
            stNetworkTimeData.hour = timeClient.getHours();
            stNetworkTimeData.minute = timeClient.getMinutes();
            stNetworkTimeData.second = timeClient.getSeconds();
            stNetworkTimeData.unix = timeClient.getEpochTime();
        }
    }
};

/**
 * MQTT callback function.
 * Processes all the receives commands from the subscribed topics
 * 
 * @parameter *topic    A pointer to a char array containing the mqtt topic that calles this function with new data
 * @parameter *payload  A pointer to a byte array with data send over the mqtt topic
 * @parameter length    The length of the byte data array
 **/
void Network::MqttCallback(char *topic, byte *payload, unsigned int length)
{

    //-- Get Message and add terminator
    char message[length + 1];    // Main data received
    char memMessage[length + 1]; // Mem of main data because main data gets changed after json deserialize
    for (unsigned int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
        memMessage[i] = (char)payload[i];
    }
    message[length] = '\0';
    memMessage[length] = '\0';

    ConfiguredData configuredData = configuration->getData();

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
            parameter_sun = (bool)data;

            if ((bool)data)
            {
                stTimeBasedMotionBrightness.sunfallUnix = stNetworkTimeData.unix;
                stTimeBasedMotionBrightness.isSunfallSet = true;
            }
            else
            {
                stTimeBasedMotionBrightness.sunriseUnix = stNetworkTimeData.unix;
                stTimeBasedMotionBrightness.isSunriseSet = true;
            }
        }
    }
    // ======== MasterPresent ======== //
    else if (String("LEDController/Global/HomeAssistant/MasterPresent/command").equals(topic))
    {
        String temp_message = strtok(message, "\0");
        if (temp_message.equals("home"))
        {
            parameter_master_present = true;
        }
        else
        {
            parameter_master_present = false;
        }
    }
    // ======== Alarm ======== //
    else if (String("LEDController/Global/HomeAssistant/Effect/Alarm/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            alarm = (bool)data;
        }
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            stNetworkMotionData.timeBasedBrightnessChangeEnabled = (bool)data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            stNetworkMotionData.motionDetectionEnabled = (bool)data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1000)
        {
            stNetworkMotionData.timeout = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            stNetworkMotionData.redColorValue = red;
            stNetworkMotionData.greenColorValue = green;
            stNetworkMotionData.blueColorValue = blue;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                stNetworkMotionData.colorBrightnessValue = 0;
            }
            else
            {
                stNetworkMotionData.colorBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            stNetworkMotionData.whiteTemperatureValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            stNetworkMotionData.whiteBrightnessValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), memMessage);
        }
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            stNetworkLedStrip1Data.power = (bool)data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            stNetworkLedStrip1Data.ledStripData.redColorValue = red;
            stNetworkLedStrip1Data.ledStripData.greenColorValue = green;
            stNetworkLedStrip1Data.ledStripData.blueColorValue = blue;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                stNetworkLedStrip1Data.ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                stNetworkLedStrip1Data.ledStripData.colorBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/command").equals(topic))
    {
        stNetworkLedStrip1Data.effect = information->StringToSingleLEDEffect(message);

        mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), memMessage);
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 2)
        {
            stNetworkLedStrip2Data.power = (bool)data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            stNetworkLedStrip2Data.ledStripData.redColorValue = red;
            stNetworkLedStrip2Data.ledStripData.greenColorValue = green;
            stNetworkLedStrip2Data.ledStripData.blueColorValue = blue;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                stNetworkLedStrip2Data.ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                stNetworkLedStrip2Data.ledStripData.colorBrightnessValue = data;
            }
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue = data;
            mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), memMessage);
        }
    }
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/command").equals(topic))
    {
        stNetworkLedStrip2Data.effect = information->StringToSingleLEDEffect(message);

        mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), memMessage);
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Virtual/PIR/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data == 1)
        {
            virtualPIRSensorTriggered = true;
        }
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
    else if (String("LEDController/" + configuredData.mqttClientName + "/JSON/MotionDetection/command").equals(topic))
    {
    }

    // ======== Strip 1 ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Strip1/command").equals(topic))
    {
    }

    // ======== Strip 2 ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Strip2/command").equals(topic))
    {
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    else if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Virtual/PIR/command").equals(topic))
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

void Network::PublishHomeassistantMotionDetected()
{
    String message = "";

    // ==== Motion Detected
    message = String(pirReader->motionDetected);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/state").c_str(), message.c_str());
    message = String(pirReader->sensorTriggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/state").c_str(), message.c_str());
    message = String(pirReader->sensor1Triggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor1/state").c_str(), message.c_str());
    message = String(pirReader->sensor2Triggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor2/state").c_str(), message.c_str());
    message = String(pirReader->virtualSensorTriggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/VirtualSensor/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantLEDStripData()
{
    String message = "";

    // ==== Strip 1
    message = String(stNetworkLedStrip1Data.power);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip1Data.ledStripData.redColorValue + String(",") + stNetworkLedStrip1Data.ledStripData.greenColorValue + String(",") + stNetworkLedStrip1Data.ledStripData.blueColorValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip1Data.ledStripData.colorBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), message.c_str());
    message = String(information->SingleLEDEffectToString(stNetworkLedStrip1Data.effect));
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), message.c_str());

    // ==== Strip 2
    message = String(stNetworkLedStrip2Data.power);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip2Data.ledStripData.redColorValue + String(",") + stNetworkLedStrip2Data.ledStripData.greenColorValue + String(",") + stNetworkLedStrip2Data.ledStripData.blueColorValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip2Data.ledStripData.colorBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), message.c_str());
    message = String(stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), message.c_str());
    message = String(information->SingleLEDEffectToString(stNetworkLedStrip2Data.effect));
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantElectricalMeasurement()
{
    String message = "";

    // ==== Electrical Measurement
    message = String(powerMeasurement->valuePower_mW);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/ElectricalMesurement/CurrentPower/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueBus_V);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/ElectricalMesurement/BusVoltage/state").c_str(), message.c_str());
    message = String(powerMeasurement->valueCurrent_mA);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/ElectricalMesurement/CurrentAmpere/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantHeartbeat()
{
    String message = "pulse";

    // ==== Heartbeat
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Heartbeat/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantMotionLEDStripData()
{
    String message = "";

    // ==== Motion
    message = String(stNetworkMotionData.motionDetectionEnabled);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/Enable/state").c_str(), message.c_str());
    message = String(stNetworkMotionData.redColorValue + String(",") + stNetworkMotionData.greenColorValue + String(",") + stNetworkMotionData.blueColorValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), message.c_str());
    message = String(stNetworkMotionData.colorBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), message.c_str());
    message = String(stNetworkMotionData.whiteTemperatureValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), message.c_str());
    message = String(stNetworkMotionData.whiteBrightnessValue);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantNetwork()
{
    String message = "";

    // ==== Network
    message = ipAddress;
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Network/IPAddress/state").c_str(), message.c_str());
    message = macAddress;
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Network/MACAddress/state").c_str(), message.c_str());
}

void Network::PublishJsonMotionDetected()
{
}

void Network::PublishJsonLEDStripData()
{
}

void Network::PublishJsonElectricalMeasurement()
{
}

void Network::PublishJsonHeartbeat()
{
}

void Network::PublishJsonMotionLEDStripData()
{
}

void Network::PublishJsonNetwork()
{
}

/**
 * Publishes information about the current motion detected state
 */
void Network::PublishMotionDetected()
{
    prevMillisPublishMotionDetected = millis();

    // == Home Assistant
    PublishHomeassistantMotionDetected();

    // == JSON
    PublishJsonMotionDetected();
}

/**
 * Publishes information about the led strips
 */
void Network::PublishLEDStripData()
{
    prevMillisPublishLEDStripData = millis();

    // == Home Assistant
    PublishHomeassistantLEDStripData();

    // == JSON
    PublishJsonLEDStripData();
}

/**
 * Publishes the electrical measurement
 */
void Network::PublishElectricalMeasurement()
{
    prevMillisPublishElectricalMeasurement = millis();

    // == Home Assistant
    PublishHomeassistantElectricalMeasurement();

    // == JSON
    PublishJsonElectricalMeasurement();
}

/**
 * Publishes a heartbeat
 */
void Network::PublishHeartbeat()
{
    prevMillisPublishHeartbeat = millis();

    // == Home Assistant
    PublishHomeassistantHeartbeat();

    // == JSON
    PublishJsonHeartbeat();
}

/**
 * Publishes information about the motion led strip data
 */
void Network::PublishMotionLEDStripData()
{
    prevMillisPublishMotionLEDStripData = millis();

    // == Home Assistant
    PublishHomeassistantMotionLEDStripData();

    // == JSON
    PublishJsonMotionLEDStripData();
}

/**
 * Publishes information about the network interface  
 */
void Network::PublishNetwork()
{
    prevMillisPublishNetwork = millis();

    // == Home Assistant
    PublishHomeassistantNetwork();

    // == JSON
    PublishJsonNetwork();
}

/**
 * Publishes the current installed code version 
 */
void Network::PublishCodeVersion()
{
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/Version").c_str(), codeVersion.c_str());
}

/**
 * @brief Request for the network componente to change into access point mode
 * 
 */
void Network::RequestChangeToAccessPointMode()
{
    if (this->isInWiFiMode && !this->isInAccessPointMode)
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
    if (this->isInAccessPointMode && !this->isInWiFiMode)
    {
        this->changeToWiFiModeRequest = true;
    }
}

/**
 * @return The current WiFi state
 */
NetworkWiFiState Network::getWiFiState()
{
    return this->wifiState;
}

/**
 * @return The current access point state
 */
NetworkAccessPointState Network::getAccessPointState()
{
    return this->accessPointState;
}

/**
 * @return The current MQTT state
 */
NetworkMQTTState Network::getMQTTState()
{
    return this->mqttState;
}

/**
 * @return The current WiFi information based on the used WiFi mode (AP or STA) 
 */
NetworkWiFiInformation Network::getWiFiInformation()
{
    NetworkWiFiInformation wiFiInformation = {};

    wiFiInformation.ipAddress = WiFi.localIP().toString().c_str();
    wiFiInformation.subnetMask = WiFi.subnetMask().toString().c_str();
    wiFiInformation.macAddress = WiFi.macAddress();
    wiFiInformation.gatewayIpAddress = WiFi.gatewayIP().toString().c_str();
    wiFiInformation.hostname = WiFi.hostname();
    wiFiInformation.inWiFiMode = this->isInWiFiMode;
    wiFiInformation.inAccessPointMode = this->isInAccessPointMode;
    wiFiInformation.isWiFiConnected = this->isWiFiConnected;

    return wiFiInformation;
}

NetworkMQTTInformation Network::getMQTTInformation()
{
    NetworkMQTTInformation mqttInformation = {};

    mqttInformation.clientName = this->filesystem.getConfigurationData().mqttClientName.c_str();
    mqttInformation.brokerIpAddress = this->filesystem.getConfigurationData().mqttBrokerIpAddress.c_str();
    mqttInformation.brokerPort = this->filesystem.getConfigurationData().mqttBrokerPort;
    mqttInformation.isMQTTConnected = this->isMQTTConnected;
    mqttInformation.clientState = this->clientState;

    return mqttInformation;
}
