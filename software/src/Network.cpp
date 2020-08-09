#include "../include/Network.h"
#include "../Main.h"


//++++ Defines for WiFi Secrets +++//
#define WZ_WIFI_SETTINGS
//#define HDS_WIFI_SETTINGS
#include <secrets_wifi.h>

//++++ Defines for MQTT Secrets +++//
#define WZ_MQTT_SETTINGS
//#define WZ_FUTURE_MQTT_SETTINGS
//#define HDS_MQTT_SETTINGS
#include <secrets_mqtt.h>

//++++ Defines for MQTT Path Secrets +++//
#define DEV_LED_CONTROLLER_MK4
#include <secrets_mqtt_paths_mk2.h>

/**
 * Empty constructor
 */ 
Network::Network()
{
    
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
    if(!init)
    {
        mqttClient.setClient(wifiMqtt);
        mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
        mqttClient.setCallback(mqttCallback);

        timeClient.begin();
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
    if(!init)
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
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(1); // Call delay(1) for the WiFi stack
        wifiState = NetworkWiFiState::SuperviseWiFiConnection;
        break;

    case NetworkWiFiState::SuperviseWiFiConnection:
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiState = NetworkWiFiState::CheckWiFiDisconnect; // Check if dc occurred
            PrevMillis_WiFiTimeout = millis();          // Set time for WiFi timeout check
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
        if(wifiConnected)
        {
            if (mqttClient.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD))
            {
                // ==== Global ==== //
                // Sun
                mqttClient.subscribe(mqtt_sun_command);   

                // Master
                mqttClient.subscribe(mqtt_master_present_command);   

                // PC
                mqttClient.subscribe(mqtt_pc_present_command);   

                // Motion 
                mqttClient.subscribe(mqtt_motion_detection_power_command);   
                mqttClient.subscribe(mqtt_motion_detection_rgb_command); 
                mqttClient.subscribe(mqtt_motion_detection_timeout_command); 

                // ==== Specific ==== //
                // Strip 1
                mqttClient.subscribe(mqtt_strip1_json_command); 

                // Strip 2  
                mqttClient.subscribe(mqtt_strip2_json_command); 

                // Publish mqtt update about current led data
                MqttUpdateAfterDc(stNetworkLedStrip1Data, mqtt_strip1_json_state);
                MqttUpdateAfterDc(stNetworkLedStrip2Data, mqtt_strip2_json_state);

                mqttState = NetworkMQTTState::SuperviseMqttConnection;
            }
        }
        break;

    case NetworkMQTTState::SuperviseMqttConnection:
        if (!mqttClient.connected())
        {
            mqttState = NetworkMQTTState::CheckMqttDisconnect; // Check if dc occurred
            PrevMillis_MQTTTimeout = millis();          // Set time for WiFi timeout check
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
    if (CurMillis_NTPTimeout - PrevMillis_NTPTimeout >= TimeOut_NTPTimeout) {
        PrevMillis_NTPTimeout = CurMillis_NTPTimeout;
        bool updateSuccessful = timeClient.update();

        if(updateSuccessful)
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
    if(mqttConnected)
    {
        unsigned long CurMillis_HeartbeatTimeout = millis();
        if (CurMillis_HeartbeatTimeout - PrevMillis_HeartbeatTimeout >= TimeOut_HeartbeatTimeout)
        {
            PrevMillis_HeartbeatTimeout = CurMillis_HeartbeatTimeout;
            mqttClient.publish(mqtt_heartbeat_state, "pulse");
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
void Network::ElectricalMeasurementUpdate(  double currentPower,
                                            double busVoltage,
                                            double busCurrent)
{
    // Create json message
    StaticJsonDocument<256> doc;
    doc["currentPower"]  = currentPower;
    doc["busVoltage"]    = busVoltage;
    doc["busCurrent"]    = busCurrent;

    // Serialize json message and send
    char message[256];
    serializeJson(doc, message);
    mqttClient.publish(mqtt_electrical_measurement_json_state, message);
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
    mqttClient.publish(mqtt_motion_detected_state, message);
};


/**
 * Updates the current ledStripData to a mqtt topic in json format
 * 
 * @parameter ledStripData  LEDStripData struct to send
 * @parameter topic         Mqtt topic to send the data to
 **/
void Network::MqttUpdateAfterDc(NetworkLEDStripData networkLedStripData,
                                const char* topic)
{
    // Create json message
    StaticJsonDocument<256> doc;
    if(networkLedStripData.power)
    {
        doc["state"] = "ON";
    }
    else
    {
        doc["state"] = "OFF";   
    }
    doc["brightness"]   = networkLedStripData.ledStripData.brightnessValue;
    doc["color"]["r"]   = networkLedStripData.ledStripData.redColorValue;
    doc["color"]["g"]   = networkLedStripData.ledStripData.greenColorValue;
    doc["color"]["b"]   = networkLedStripData.ledStripData.blueColorValue;
    doc["white_value"]  = networkLedStripData.ledStripData.cwColorValue;
    doc["effect"]       = SingleLEDEffectToString(networkLedStripData.effect);
    
    // Serialize json message and send
    char message[256];
    serializeJson(doc, message);
    mqttClient.publish(topic, message);
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
    if(effect == "None")
    {
        return SingleLEDEffect::None;
    }
    else if(effect == "Alarm")
    {
        return SingleLEDEffect::Alarm;
    }
    else if(effect == "Music")
    {
        return SingleLEDEffect::Music;
    }
    else if(effect == "Sleep")
    {
        return SingleLEDEffect::Sleep;
    }
    else if(effect == "Weekend")
    {
        return SingleLEDEffect::Weekend;
    }
    else if(effect == "RGB")
    {
        return SingleLEDEffect::RGB;
    }
    else if(effect == "CW")
    {
        return SingleLEDEffect::CW;
    }
    else if(effect == "WW")
    {
        return SingleLEDEffect::WW;
    }
    else if(effect == "RGBCW")
    {
        return SingleLEDEffect::RGBCW;
    }
    else if(effect == "RGBWW")
    {
        return SingleLEDEffect::RGBWW;
    }
    else if(effect == "CWWW")
    {
        return SingleLEDEffect::CWWW;
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
    switch(effect)
    {

        case SingleLEDEffect::None:
            return "None";
            break;

        case SingleLEDEffect::Alarm:
            return "Alarm";
            break;

        case SingleLEDEffect::Music:
            return "Music";
            break;

        case SingleLEDEffect::Sleep:
            return "Sleep";
            break;

        case SingleLEDEffect::Weekend:
            return "Weekend";
            break;

        case SingleLEDEffect::RGB:
            return "RGB";
            break;

        case SingleLEDEffect::CW:
            return "CW";
            break;

        case SingleLEDEffect::WW:
            return "WW";
            break;

        case SingleLEDEffect::RGBCW:
            return "RGBCW";
            break;

        case SingleLEDEffect::RGBWW:
            return "RGBWW";
            break;

        case SingleLEDEffect::CWWW:
            return "CWWW";
            break;

        default:
            return "None";
            break;

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
    char message[length + 1];       // Main data received
    char memMessage[length + 1];    // Mem of main data because main data gets changed after json deserialize
    for (int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
        memMessage[i] = (char)payload[i];
    }
    message[length] = '\0';
    memMessage[length] = '\0';

    
    //######################################## mqtt_sun_command ########################################//
    if (String(mqtt_sun_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.parameter_sun = (bool)data;
        }
    }

    //######################################## mqtt_master_present_command ########################################//
    if (String(mqtt_master_present_command).equals(topic))
    {
        String temp_message = strtok(message, "\0");
        if(temp_message.equals("home"))
        {
            mainController.network.parameter_master_present = true;
        }
        else
        {
            mainController.network.parameter_master_present = false;
        }
    }

    //######################################## mqtt_pc_present_command ########################################//
    if (String(mqtt_pc_present_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.parameter_pc_present = (bool)data;
        }
    }

    //######################################## mqtt_motion_detection_power_command ########################################//
    if (String(mqtt_motion_detection_power_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.stNetworkMotionData.power = (bool)data;
        }
    }
    
    //######################################## mqtt_motion_detection_rgb_command ########################################//
    if (String(mqtt_motion_detection_rgb_command).equals(topic))
    {
        uint8_t red = atoi(strtok(message, ",")); 
        if (red >= 0 && red <= 255)
        {
            mainController.network.stNetworkMotionData.redColorValue = red;
        }
        uint8_t green = atoi(strtok(NULL, ",")); 
        if (green >= 0 && green <= 255)
        {
            mainController.network.stNetworkMotionData.greenColorValue = green;
        }
        uint8_t blue = atoi(strtok(NULL, ",")); 
        if (blue >= 0 && blue <= 255)
        {
            mainController.network.stNetworkMotionData.blueColorValue = blue;
        }
    }

    //######################################## mqtt_motion_detection_timeout_command ########################################//
    if (String(mqtt_motion_detection_timeout_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1000)
        {
            mainController.network.stNetworkMotionData.timeout = data;
        }
    }
    
    //######################################## mqtt_strip1_json_command ########################################//
    if (String(mqtt_strip1_json_command).equals(topic))
    {

        // Deserialize message
        DeserializationError error = deserializeJson(mainController.network.doc, message);
        if(error)
        {
            Serial.print("Deserialize JSON failed!");
            Serial.println(error.c_str());
            return;
        }

        // ======== Power ======== //
        JsonVariant state = mainController.network.doc["state"]; 
        if(!state.isNull())
        {
            if(mainController.network.doc["state"] == "ON")
            {
                mainController.network.stNetworkLedStrip1Data.power = true;
            }
            else
            {
                mainController.network.stNetworkLedStrip1Data.power = false;
            }
        }

        // ======== Brightness ======== //
        JsonVariant brightness = mainController.network.doc["brightness"]; 
        if(!brightness.isNull())
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.brightnessValue = brightness.as<uint16_t>();
        }

        // ======== White Value ======== //
        JsonVariant white_value = mainController.network.doc["white_value"]; 
        if(!white_value.isNull())
        {
            mainController.network.stNetworkLedStrip1Data.ledStripData.cwColorValue = white_value.as<uint8_t>();
        }

        // ======== Color ======== //
        JsonVariant color = mainController.network.doc["color"]; 
        if(!color.isNull())
        {
            // ==== Red ==== //
            JsonVariant r = mainController.network.doc["color"]["r"]; 
            if(!r.isNull())
            {
                mainController.network.stNetworkLedStrip1Data.ledStripData.redColorValue = r.as<uint8_t>();
            }
            // ==== Green ==== //
            JsonVariant g = mainController.network.doc["color"]["g"]; 
            if(!g.isNull())
            {
                mainController.network.stNetworkLedStrip1Data.ledStripData.greenColorValue = g.as<uint8_t>();
            }
            // ==== Blue ==== //
            JsonVariant b = mainController.network.doc["color"]["b"]; 
            if(!b.isNull())
            {
                mainController.network.stNetworkLedStrip1Data.ledStripData.blueColorValue = b.as<uint8_t>();
            }
        }

        // ======== Effect ======== //
        JsonVariant effect = mainController.network.doc["effect"]; 
        if(!effect.isNull())
        {
            mainController.network.stNetworkLedStrip1Data.effect = StringToSingleLEDEffect(effect.as<String>());
        }

        // Send message back to mqtt state topic
        mainController.network.mqttClient.publish(mqtt_strip1_json_state, memMessage);
    }

    //######################################## mqtt_strip2_json_command ########################################//
    if (String(mqtt_strip2_json_command).equals(topic))
    {
        // Deserialize message
        DeserializationError error = deserializeJson(mainController.network.doc, message);
        if(error)
        {
            Serial.print("Deserialize JSON failed!");
            Serial.println(error.c_str());
            return;
        }

        // ======== Power ======== //
        JsonVariant state = mainController.network.doc["state"]; 
        if(!state.isNull())
        {
            if(mainController.network.doc["state"] == "ON")
            {
                mainController.network.stNetworkLedStrip2Data.power = true;
            }
            else
            {
                mainController.network.stNetworkLedStrip2Data.power = false;
            }
        }

        // ======== Brightness ======== //
        JsonVariant brightness = mainController.network.doc["brightness"]; 
        if(!brightness.isNull())
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.brightnessValue = brightness.as<uint16_t>();
        }

        // ======== White Value ======== //
        JsonVariant white_value = mainController.network.doc["white_value"]; 
        if(!white_value.isNull())
        {
            mainController.network.stNetworkLedStrip2Data.ledStripData.cwColorValue = white_value.as<uint8_t>();
        }

        // ======== Color ======== //
        JsonVariant color = mainController.network.doc["color"]; 
        if(!color.isNull())
        {
            // ==== Red ==== //
            JsonVariant r = mainController.network.doc["color"]["r"]; 
            if(!r.isNull())
            {
                mainController.network.stNetworkLedStrip2Data.ledStripData.redColorValue = r.as<uint8_t>();
            }
            // ==== Green ==== //
            JsonVariant g = mainController.network.doc["color"]["g"]; 
            if(!g.isNull())
            {
                mainController.network.stNetworkLedStrip2Data.ledStripData.greenColorValue = g.as<uint8_t>();
            }
            // ==== Blue ==== //
            JsonVariant b = mainController.network.doc["color"]["b"]; 
            if(!b.isNull())
            {
                mainController.network.stNetworkLedStrip2Data.ledStripData.blueColorValue = b.as<uint8_t>();
            }
        }

        // ======== Effect ======== //
        JsonVariant effect = mainController.network.doc["effect"]; 
        if(!effect.isNull())
        {
            mainController.network.stNetworkLedStrip2Data.effect = StringToSingleLEDEffect(effect.as<String>());
        }

        // Send message back to mqtt state topic
        mainController.network.mqttClient.publish(mqtt_strip2_json_state, memMessage);
    }

}


