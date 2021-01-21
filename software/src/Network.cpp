#include "../include/Network.h"
#include "../Main.h"

/**
 * Empty constructor
 */
Network::Network(){

};

// For compiler
void mqttCallback(char *topic, byte *payload, unsigned int length);
SingleLEDEffect StringToSingleLEDEffect(String effect);
String SingleLEDEffectToString(SingleLEDEffect effect);

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

#ifdef ENABLE_PC_PRESENT_MOTION_DISABLE
        enable_pc_present = true;
#endif

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
            Serial.println(WiFi.localIP());
            Serial.print("  Subnetmask  : ");
            Serial.println(WiFi.subnetMask());
            Serial.print("  MAC Address : ");
            Serial.println(WiFi.macAddress());
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

    // -- Heartbeat
    Heartbeat();
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

                // Effects
                mqttClient.subscribe("LEDController/Global/HomeAssistant/Effect/command");

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
                mqttClient.subscribe(("LEDController/" + data.mqttClientName + "/HomeAssistant/Virtual/PIR/command").c_str());

                // === Republish == //
                //MqttUpdateAfterDc(stNetworkLedStrip1Data, mqtt_strip1_json_state);
                //MqttUpdateAfterDc(stNetworkLedStrip2Data, mqtt_strip2_json_state);

                // ================ Json ================ //
                /*
                    These Mqtt paths are for the paths given for the desired behavior when controlloing over custom json data
                */
                // ==== Global ==== //
                mqttClient.subscribe("LEDController/Global/JSON/Sun/command");

                // ==== Specific ==== //

                // === Republish == //

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
 * Publishes a heartbeat update to the defined mqtt path every few milliseconds if mqtt is available
 */
void Network::Heartbeat()
{
    ConfiguredData configuredData = mainController.configuration.getData();

    if (mqttConnected)
    {
        unsigned long CurMillis_HeartbeatTimeout = millis();
        if (CurMillis_HeartbeatTimeout - PrevMillis_HeartbeatTimeout >= TimeOut_HeartbeatTimeout)
        {
            PrevMillis_HeartbeatTimeout = CurMillis_HeartbeatTimeout;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Heartbeat//state").c_str(), "pulse");
        }
    }
};

/**
 * Publishes a electrical measurement update in json format
 * 
 * @parameter currentPower  The value of the power in mW to send
 * @parameter busVoltage    The value of the bus voltage in V to send
 * @parameter busCurrent    The value of the bus current in mA
 **/
void Network::ElectricalMeasurementUpdate(double currentPower,
                                          double busVoltage,
                                          double busCurrent)
{
    // Create json message
    StaticJsonDocument<256> doc;
    doc["currentPower"] = currentPower;
    doc["busVoltage"] = busVoltage;
    doc["busCurrent"] = busCurrent;

    // Serialize json message and send
    char message[256];
    serializeJson(doc, message);
    // ToDO implement
    //mqttClient.publish(mqtt_electrical_measurement_json_state, message);
};

/**
 * Publishes a motion detection update to the defined mqtt path
 * 
 * @parameter motion    The value of the current detected motion
 **/
void Network::MotionDetectedUpdate(bool motion)
{
    char message[8];
    message[0] = motion;
    // ToDO implement
    //mqttClient.publish(mqtt_motion_detected_state, message);
};

/**
 * Updates the current ledStripData to a mqtt topic in json format
 * 
 * @parameter ledStripData  LEDStripData struct to send
 * @parameter topic         Mqtt topic to send the data to
 **/
void Network::MqttUpdateAfterDc(NetworkLEDStripData networkLedStripData,
                                const char *topic)
{
    // Create json message
    StaticJsonDocument<256> doc;
    if (networkLedStripData.power)
    {
        doc["state"] = "ON";
    }
    else
    {
        doc["state"] = "OFF";
    }
    doc["brightness"] = networkLedStripData.ledStripData.colorBrightnessValue;
    doc["color"]["r"] = networkLedStripData.ledStripData.redColorValue;
    doc["color"]["g"] = networkLedStripData.ledStripData.greenColorValue;
    doc["color"]["b"] = networkLedStripData.ledStripData.blueColorValue;
    doc["white_value"] = networkLedStripData.ledStripData.whiteTemperatureValue;
    doc["effect"] = SingleLEDEffectToString(networkLedStripData.effect);

    // Serialize json message and send
    char message[256];
    serializeJson(doc, message);
    mqttClient.publish(topic, message);
    // ToDO fix / recode
};

/**
 * Converts a string to a LEDEffect
 * 
 * @parameter effect    The name of effect as string
 * 
 * @return effect The corresponding LEDEffect to the given string effect
 **/
SingleLEDEffect StringToSingleLEDEffect(String effect)
{
    if (effect == "None")
    {
        return SingleLEDEffect::None;
    }
    else if (effect == "TriplePulse")
    {
        return SingleLEDEffect::TriplePulse;
    }
    else if (effect == "Rainbow")
    {
        return SingleLEDEffect::Rainbow;
    }
    else // default
    {
        return SingleLEDEffect::None;
    }
};

/**
 * Converts a LEDEffect to a String
 * 
 * @parameter effect    The LEDEffect to convert to string
 * 
 * @return effect The corresponding string effect to the given LEDEffect
 **/
String SingleLEDEffectToString(SingleLEDEffect effect)
{
    switch (effect)
    {

    case SingleLEDEffect::None:
        return "None";
        break;

    case SingleLEDEffect::TriplePulse:
        return "TriplePulse";
        break;

    case SingleLEDEffect::Rainbow:
        return "Rainbow";
        break;

    default:
        return "None";
        break;
    }
};

/**
 * Converts a string to a AlarmMode
 * 
 * @parameter mode    The name of mode as string
 * 
 * @return mode The corresponding AlarmMode to the given string mode
 **/
AlarmMode StringToAlarmMode(String mode)
{
    if (mode == "Nothing")
    {
        return AlarmMode::Nothing;
    }
    else if (mode == "Warning")
    {
        return AlarmMode::Warning;
    }
    else if (mode == "Error")
    {
        return AlarmMode::Error;
    }
    else if (mode == "Critical")
    {
        return AlarmMode::Critical;
    }
    else // default
    {
        return AlarmMode::Nothing;
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
    for (int i = 0; i < length; i++)
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
        uint8_t data = atoi(message);
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
    // ======== Effects ======== //
    if (String("LEDController/Global/HomeAssistant/Effect/command").equals(topic))
    {
        // TODO implement
    }

    // ================ Specific ================ //
    // ======== Motion ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/command").equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkMotionData.timeBasedBrightnessChangeEnabled = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/BrightnessTimeBasedEnabled/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/command").equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkMotionData.motionDetectionEnabled = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Enabled/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/command").equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1000)
        {
            mainController.network.stNetworkMotionData.timeout = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/Timeout/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/RGB/command").equals(topic))
    {
        uint8_t red = atoi(strtok(message, ","));
        uint8_t green = atoi(strtok(NULL, ","));
        uint8_t blue = atoi(strtok(NULL, ","));
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
        uint16_t data = atoi(message);
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
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkMotionData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/command").equals(topic))
    {
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkMotionData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/MotionDetection/White/Brightness/state").c_str(), memMessage);
        }
    }

    // ======== Strip 1 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/command").equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkLedStrip1Data.power = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Power/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/RGB/command").equals(topic))
    {
        uint8_t red = atoi(strtok(message, ","));
        uint8_t green = atoi(strtok(NULL, ","));
        uint8_t blue = atoi(strtok(NULL, ","));
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
        uint16_t data = atoi(message);
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
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/command").equals(topic))
    {
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/White/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/command").equals(topic))
    {
        mainController.network.stNetworkLedStrip1Data.effect = StringToSingleLEDEffect(message);

        mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip1/Effect/state").c_str(), memMessage);
    }

    // ======== Strip 2 ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/command").equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 2)
        {
            mainController.network.stNetworkLedStrip2Data.power = (bool)data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Power/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/RGB/command").equals(topic))
    {
        uint8_t red = atoi(strtok(message, ","));
        uint8_t green = atoi(strtok(NULL, ","));
        uint8_t blue = atoi(strtok(NULL, ","));
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
        uint16_t data = atoi(message);
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
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 500)
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.whiteTemperatureValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/command").equals(topic))
    {
        uint16_t data = atoi(message);
        if (data >= 0 && data <= 4095)
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.whiteBrightnessValue = data;
            mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/White/Brightness/state").c_str(), memMessage);
        }
    }
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/command").equals(topic))
    {
        mainController.network.stNetworkLedStrip2Data.effect = StringToSingleLEDEffect(message);

        mainController.network.mqttClient.publish(("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Strip2/Effect/state").c_str(), memMessage);
    }

    // ================ Virtual ================ //
    // ======== PIR ======== //
    if (String("LEDController/" + configuredData.mqttClientName + "/HomeAssistant/Virtual/PIR/command").equals(topic))
    {
        // TODO Implement
    }

    // # ================================ JSON ================================ //
    /*
        Callback stuff for data received by json endpoints
    */

    // ================ Global ================ //
    // ======== Sun ======== //

    // ======== MasterPresent ======== //

    // ======== Effects ======== //

    // ================ Specific ================ //
    // ======== Motion ======== //

    // ======== Strip 1 ======== //

    // ======== Strip 2 ======== //

    // ================ Virtual ================ //
    // ======== PIR ======== //
}
