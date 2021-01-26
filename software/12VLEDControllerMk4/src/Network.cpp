#include "../include/Network.h"
#include "../Main.h"

void mqttCallback(char *topic, byte *payload, unsigned int length);

/**
 * Empty constructor
 */
Network::Network(){

};

/**
 * Does init stuff for the Network component
 * 
 * @return True if successfull, false if not 
 */
bool Network::Init()
{
    if (!init)
    {
        data = mainController.configuration.getData();

        mqttClient.setClient(wifiMqtt);
        mqttClient.setServer(data.mqttBrokerIpAddress.c_str(),
                             data.mqttBrokerPort);
        mqttClient.setCallback(mqttCallback);

        timeClient.begin();

        wifiState = NetworkWiFiState::StartWifi;
        mqttState = NetworkMQTTState::StartMqtt;

        virtualPIRSensorTriggered = false;

        Serial.println("Network initialized");
        init = true;
    }

    return init;
};

/**
 * Runs the Network component. 
 */
void Network::Run()
{
    if (!init)
    {
        return;
    }

    mqttClient.loop();

    // -- Wifi
    HandleWifi();
    if (wifiConnected != memWifiConnected)
    {
        memWifiConnected = wifiConnected;
        wifiOneTimePrint = true;
    }
    if (wifiOneTimePrint)
    {
        if (wifiConnected)
        {
            Serial.println("\n-- Wifi Connected --");
            Serial.print("  IP Address  : ");
            ipAddress = WiFi.localIP().toString().c_str();
            Serial.println(ipAddress);
            Serial.print("  Subnetmask  : ");
            Serial.println(WiFi.subnetMask());
            Serial.print("  MAC Address : ");
            macAddress = WiFi.macAddress();
            Serial.println(macAddress);
            Serial.print("  Gateway     : ");
            Serial.println(WiFi.gatewayIP());
        }
        else
        {
            Serial.println("\n-- Wifi Disconnected --");
        }
        wifiOneTimePrint = false;
    }

    // -- Mqtt
    HandleMqtt();
    if (mqttConnected != memMqttConnected)
    {
        memMqttConnected = mqttConnected;
        mqttOneTimePrint = true;
    }
    if (mqttOneTimePrint)
    {
        if (mqttConnected)
        {
            Serial.println("\n-- Mqtt Connected --");
        }
        else
        {
            Serial.println("\n-- Mqtt Disconnected --");
        }
        mqttOneTimePrint = false;
    }

    // -- NTP
    HandleNTP();

    // -- Republish
    HandleRepublish();
};

/**
 * Handels the WiFi connection.
 * Auto reconnects on dc
 */
void Network::HandleWifi()
{
    switch (wifiState)
    {
    case NetworkWiFiState::StartWifi:
        WiFi.mode(WIFI_STA);
        WiFi.begin(data.wifiSSID.c_str(),
                   data.wifiPassword.c_str());
        delay(1); // Call delay(1) for the WiFi stack
        wifiState = NetworkWiFiState::SuperviseWiFiConnection;
        break;

    case NetworkWiFiState::SuperviseWiFiConnection:
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiState = NetworkWiFiState::CheckWiFiDisconnect; // Check if dc occurred
            PrevMillis_WiFiTimeout = millis();                 // Set time for WiFi timeout check
        }
        else
        {
            wifiConnected = true;
        }
        break;

    case NetworkWiFiState::CheckWiFiDisconnect:
        if (WiFi.status() != WL_CONNECTED)
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_WiFiTimeout = millis();
            if (CurMillis_WiFiTimeout - PrevMillis_WiFiTimeout >= TimeOut_WiFiTimeout)
            {
                wifiConnected = false;
                PrevMillis_WiFiTimeout = CurMillis_WiFiTimeout;
                WiFi.disconnect(); // Disconnect WiFi and start new connection
                wifiState = NetworkWiFiState::StartWifi;
            }
        }
        else
        {
            wifiState = NetworkWiFiState::SuperviseWiFiConnection; // WiFi reconnected
        }
        break;

    default:
        Serial.println("Wifi State Error!");
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

    switch (mqttState)
    {
    case NetworkMQTTState::StartMqtt:
        // Only try reconnect when WiFi is connected
        if (wifiConnected)
        {
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
        Serial.println("Mqtt State Error!");
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
void mqttCallback(char *topic, byte *payload, unsigned int length)
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

    ConfiguredData configuredData = mainController.configuration.getData();

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
            mainController.network.parameter_sun = (bool)data;
        }
    }
    // ======== MasterPresent ======== //
    if (String("LEDController/Global/HomeAssistant/MasterPresent/command").equals(topic))
    {
        String temp_message = strtok(message, "\0");
        if (temp_message.equals("home"))
        {
            mainController.network.parameter_master_present = true;
        }
        else
        {
            mainController.network.parameter_master_present = false;
        }
    }
    // ======== Alarm ======== //
    if (String("LEDController/Global/HomeAssistant/Effect/Alarm/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            mainController.network.alarm = (bool)data;
        }
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkMotionData.timeBasedBrightnessChangeEnabled = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkMotionData.motionDetectionEnabled = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1000)
        {
            mainController.network.stNetworkMotionData.timeout = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            mainController.network.stNetworkMotionData.redColorValue = red;
            mainController.network.stNetworkMotionData.greenColorValue = green;
            mainController.network.stNetworkMotionData.blueColorValue = blue;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                mainController.network.stNetworkMotionData.colorBrightnessValue = 0;
            }
            else
            {
                mainController.network.stNetworkMotionData.colorBrightnessValue = data;
            }
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkMotionData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkMotionData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), memMessage);
        }
    }

    // ======== Strip 1 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkLedStrip1Data.power = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.redColorValue = red;
            mainController.network.stNetworkLedStrip1Data.ledStripData.greenColorValue = green;
            mainController.network.stNetworkLedStrip1Data.ledStripData.blueColorValue = blue;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                mainController.network.stNetworkLedStrip1Data.ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                mainController.network.stNetworkLedStrip1Data.ledStripData.colorBrightnessValue = data;
            }
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/command").equals(topic))
    {
        mainController.network.stNetworkLedStrip1Data.effect = mainController.information.StringToSingleLEDEffect(message);

        mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), memMessage);
    }

    // ======== Strip 2 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 2)
        {
            mainController.network.stNetworkLedStrip2Data.power = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/command").equals(topic))
    {
        long int red = strtol(strtok(message, ","), NULL, 10);
        long int green = strtol(strtok(NULL, ","), NULL, 10);
        long int blue = strtol(strtok(NULL, ","), NULL, 10);
        if ((red >= 0 && red <= 255) && (green >= 0 && green <= 255) && (blue >= 0 && blue <= 255))
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.redColorValue = red;
            mainController.network.stNetworkLedStrip2Data.ledStripData.greenColorValue = green;
            mainController.network.stNetworkLedStrip2Data.ledStripData.blueColorValue = blue;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            // Because home assistant rgb brightness slider is retarded and can only go to 1% and not to 0% we set the brightness to zero if below 50
            if (data <= 50)
            {
                mainController.network.stNetworkLedStrip2Data.ledStripData.colorBrightnessValue = 0;
            }
            else
            {
                mainController.network.stNetworkLedStrip2Data.ledStripData.colorBrightnessValue = data;
            }
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/command").equals(topic))
    {
        mainController.network.stNetworkLedStrip2Data.effect = mainController.information.StringToSingleLEDEffect(message);

        mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), memMessage);
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Virtual/PIR/command").equals(topic))
    {
        long int data = strtol(message, NULL, 10);
        if (data == 1)
        {
            mainController.network.virtualPIRSensorTriggered = true;
        }
    }

    // # ================================ JSON ================================ //
    /*
        Callback stuff for data received by json endpoints
    */

    // ================ Global ================ //
    // ======== Sun ======== //
    if (String("LEDController/Global/JSON/Sun/command").equals(topic))
    {
    }
    // ======== MasterPresent ======== //
    if (String("LEDController/Global/JSON/MasterPresent/command").equals(topic))
    {
    }

    // ======== Effects ======== //
    // ======== Alarm ======== //
    if (String("LEDController/Global/JSON/Effect/Alarm/command").equals(topic))
    {
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/JSON/MotionDetection/command").equals(topic))
    {
    }

    // ======== Strip 1 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Strip1/command").equals(topic))
    {
    }

    // ======== Strip 2 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Strip2/command").equals(topic))
    {
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/JSON/Virtual/PIR/command").equals(topic))
    {
    }
}

void Network::HandleRepublish()
{
    // For now disabled
    return;

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
    message = String(mainController.pirReader.motionDetected);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/state").c_str(), message.c_str());
    message = String(mainController.pirReader.sensorTriggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/state").c_str(), message.c_str());
    message = String(mainController.pirReader.sensor1Triggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor1/state").c_str(), message.c_str());
    message = String(mainController.pirReader.sensor2Triggered);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Motion/PIR/Sensor2/state").c_str(), message.c_str());
    message = String(mainController.pirReader.virtualSensorTriggered);
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
    message = String(mainController.information.SingleLEDEffectToString(stNetworkLedStrip1Data.effect));
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
    message = String(mainController.information.SingleLEDEffectToString(stNetworkLedStrip2Data.effect));
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), message.c_str());
}

void Network::PublishHomeassistantElectricalMeasurement()
{
    String message = "";

    // ==== Electrical Measurement
    message = String(mainController.powerMessurement.valuePower_mW);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/ElectricalMesurement/CurrentPower/state").c_str(), message.c_str());
    message = String(mainController.powerMessurement.valueBus_V);
    mqttClient.publish(("LEDController/" + data.mqttClientName + "/HomeAssistant/ElectricalMesurement/BusVoltage/state").c_str(), message.c_str());
    message = String(mainController.powerMessurement.valueCurrent_mA);
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
