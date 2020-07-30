#include "Network.h"
#include "Main.h"

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

using namespace LedControllerSoftwareMk5;


Network::Network()
{

};


// For compiler
void mqttCallback(char *topic, byte *payload, unsigned int length);


/**
 * Initializes the network instance
 * @parameter None
 * @return None
 **/
void Network::Init()
{
    mqttClient.setClient(wifiMqtt);
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    Serial.println("Network initialized");
};


/**
 * Needs to get called every cycle. 
 * Handels all the network stuff (WiFi, MQTT and Heartbeat)
 * @parameter None
 * @return None
 **/
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

    // -- Heartbeat
    Heartbeat();
};


/**
 * Needs to get called every cycle. 
 * Handels the WiFi connection
 * @parameter None
 * @return None
 **/
void Network::HandleWifi()
{
    switch (wifiState)
    {
    case WiFiState::StartWifi:
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(1); // Call delay(1) for the WiFi stack
        wifiState = WiFiState::SuperviseWiFiConnection;
        break;

    case WiFiState::SuperviseWiFiConnection:
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiState = WiFiState::CheckWiFiDisconnect; // Check if dc occurred
            PrevMillis_WiFiTimeout = millis();          // Set time for WiFi timeout check
        }
        else
        {
            wifiConnected = true;
        }
        break;

    case WiFiState::CheckWiFiDisconnect:
        if (WiFi.status() != WL_CONNECTED)
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_WiFiTimeout = millis();
            if (CurMillis_WiFiTimeout - PrevMillis_WiFiTimeout >= TimeOut_WiFiTimeout)
            {
                wifiConnected = false;
                PrevMillis_WiFiTimeout = CurMillis_WiFiTimeout;
                WiFi.disconnect(); // Disconnect WiFi and start new connection
                wifiState = WiFiState::StartWifi;
            }
        }
        else
        {
            wifiState = WiFiState::SuperviseWiFiConnection; // WiFi reconnected
        }
        break;

    default:
        Serial.println("Wifi State Error!");
        break;
    }
};


/**
 * Needs to get called every cycle. 
 * Handels the MQTT connection after the wifi is connected
 * Subscribes to a list pre defined topics
 * @parameter None
 * @return None
 **/
void Network::HandleMqtt()
{
 
    switch (mqttState)
    {
    case MQTTState::StartMqtt:
        if (mqttClient.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD))
        {
            // ==== Global ==== //
            // Sun
            mqttClient.subscribe(mqtt_sun_command);   

            // Time
            mqttClient.subscribe(mqtt_time_command);   

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

            mqttState = MQTTState::SuperviseMqttConnection;
        }
        break;

    case MQTTState::SuperviseMqttConnection:
        if (!mqttClient.connected())
        {
            mqttState = MQTTState::CheckMqttDisconnect; // Check if dc occurred
            PrevMillis_MQTTTimeout = millis();          // Set time for WiFi timeout check
        }
        else
        {
            mqttConnected = true;
        }
        break;

    case MQTTState::CheckMqttDisconnect:
        if (!mqttClient.connected())
        {
            // Wait for timeout. After timeout restart WiFi
            unsigned long CurMillis_MQTTTimeout = millis();
            if (CurMillis_MQTTTimeout - PrevMillis_MQTTTimeout >= TimeOut_MQTTTimeout)
            {
                mqttConnected = false;
                PrevMillis_MQTTTimeout = CurMillis_MQTTTimeout;
                mqttClient.disconnect(); // Disconnect MQTT and start new connection
                mqttState = MQTTState::StartMqtt;
            }
        }
        else
        {
            mqttState = MQTTState::SuperviseMqttConnection; // WiFi reconnected
        }
        break;

    default:
        Serial.println("Mqtt State Error!");
        break;
    }
};


/**
 * Publishes a heartbeat update to the defined mqtt path,
 * if mqtt is available
 * @parameter None
 * @return Power value
 **/
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
 * Publishes a electrical measurement power update to the defined mqtt path
 * @parameter power value in mW
 * @return Power value
 **/
void Network::ElectricalMeasurementPowerUpdate(double powerValue)
{
    char message[8];
    dtostrf(powerValue, 6, 2, message);
    mqttClient.publish(mqtt_electrical_measurement_power_state, message);
};


/**
 * Publishes a electrical measurement voltage update to the defined mqtt path
 * @parameter bus voltage in volt
 * @return Voltage value
 **/
void Network::ElectricalMeasurementVoltageUpdate(double voltageValue)
{
    char message[8];
    dtostrf(voltageValue, 6, 2, message);
    mqttClient.publish(mqtt_electrical_measurement_voltage_state, message);
};


/**
 * Publishes a electrical measurement current update to the defined mqtt path
 * @parameter current in mA
 * @return Current value
 **/
void Network::ElectricalMeasurementCurrentUpdate(double currentValue)
{
    char message[8];
    dtostrf(currentValue, 6, 2, message);
    mqttClient.publish(mqtt_electrical_measurement_current_state, message);
};


/**
 * Publishes a motion detection update to the defined mqtt path
 * @parameter Motion detected or not
 * @return Current value
 **/
void Network::MotionDetectedUpdate(bool motion)
{
    char message[8];
    message[0] = motion;
    mqttClient.publish(mqtt_motion_detected_state, message);
};


/**
 * Converts a string to a LEDEffect
 * @parameter name of effect in string
 * @return effect in LEDEffect
 **/
LedControllerSoftwareMk5::LEDEffect StringToLEDEffect(String effect)
{

    if(effect == "None")
    {
        return LedControllerSoftwareMk5::LEDEffect::None;
    }
    if(effect == "Alarm")
    {
        return LedControllerSoftwareMk5::LEDEffect::Alarm;
    }
    if(effect == "Music")
    {
        return LedControllerSoftwareMk5::LEDEffect::Music;
    }
    if(effect == "Sleep")
    {
        return LedControllerSoftwareMk5::LEDEffect::Sleep;
    }
    if(effect == "Weekend")
    {
        return LedControllerSoftwareMk5::LEDEffect::Weekend;
    }
    if(effect == "RGB")
    {
        return LedControllerSoftwareMk5::LEDEffect::RGB;
    }
    if(effect == "CW")
    {
        return LedControllerSoftwareMk5::LEDEffect::CW;
    }
    if(effect == "WW")
    {
        return LedControllerSoftwareMk5::LEDEffect::WW;
    }
    if(effect == "RGBCW")
    {
        return LedControllerSoftwareMk5::LEDEffect::RGBCW;
    }
    if(effect == "RGBWW")
    {
        return LedControllerSoftwareMk5::LEDEffect::RGBWW;
    }
    if(effect == "CWWW")
    {
        return LedControllerSoftwareMk5::LEDEffect::CWWW;
    }

};


/**
 * MQTT callback function.
 * Processes all the received commands from the subscribed topics
 * @parameter None
 * @return None
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

    //######################################## mqtt_time_command ########################################//
    if (String(mqtt_time_command).equals(topic))
    {
        std::string hour_str    = strtok(message, ":");
        std::string minute_str  = strtok(NULL, "\0"); 
        mainController.network.stTimeData.hour = atoi(hour_str.c_str());
        mainController.network.stTimeData.minute = atoi(minute_str.c_str());
    }

    //######################################## mqtt_master_present_command ########################################//
    if (String(mqtt_master_present_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1)
        {
            mainController.network.parameter_master_present = (bool)data;
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
            mainController.network.stMotionData.power = (bool)data;
        }
    }
    
    //######################################## mqtt_motion_detection_rgb_command ########################################//
    if (String(mqtt_motion_detection_rgb_command).equals(topic))
    {
        uint8_t red = atoi(strtok(message, ",")); 
        if (red >= 0 && red <= 255)
        {
            mainController.network.stMotionData.red = red;
        }
        uint8_t green = atoi(strtok(NULL, ",")); 
        if (green >= 0 && green <= 255)
        {
            mainController.network.stMotionData.green = green;
        }
        uint8_t blue = atoi(strtok(NULL, ",")); 
        if (blue >= 0 && blue <= 255)
        {
            mainController.network.stMotionData.blue = blue;
        }
    }

    //######################################## mqtt_motion_detection_timeout_command ########################################//
    if (String(mqtt_motion_detection_timeout_command).equals(topic))
    {
        uint8_t data = atoi(message);
        if (data >= 0 && data <= 1000)
        {
            mainController.network.stMotionData.timeout = data;
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
                mainController.network.stLedStrip1Data.power = true;
            }
            else
            {
                mainController.network.stLedStrip1Data.power = false;
            }
        }

        // ======== Brightness ======== //
        JsonVariant brightness = mainController.network.doc["brightness"]; 
        if(!brightness.isNull())
        {
            mainController.network.stLedStrip1Data.brightness = brightness.as<uint16_t>();
        }

        // ======== White Value ======== //
        JsonVariant white_value = mainController.network.doc["white_value"]; 
        if(!white_value.isNull())
        {
            mainController.network.stLedStrip1Data.cw = white_value.as<uint8_t>();
        }

        // ======== Color ======== //
        JsonVariant color = mainController.network.doc["color"]; 
        if(!color.isNull())
        {
            // ==== Red ==== //
            JsonVariant r = mainController.network.doc["color"]["r"]; 
            if(!r.isNull())
            {
                mainController.network.stLedStrip1Data.red = r.as<uint8_t>();
            }
            // ==== Green ==== //
            JsonVariant g = mainController.network.doc["color"]["g"]; 
            if(!g.isNull())
            {
                mainController.network.stLedStrip1Data.green = g.as<uint8_t>();
            }
            // ==== Blue ==== //
            JsonVariant b = mainController.network.doc["color"]["b"]; 
            if(!b.isNull())
            {
                mainController.network.stLedStrip1Data.blue = b.as<uint8_t>();
            }
        }

        // ======== Effect ======== //
        JsonVariant effect = mainController.network.doc["effect"]; 
        if(!effect.isNull())
        {
            mainController.network.stLedStrip1Data.effect = StringToLEDEffect(effect.as<String>());
        }

        // Send message back to mqtt state topic
        mainController.network.mqttClient.publish(mqtt_strip1_json_state, memMessage);
    }

    //######################################## mqtt_strip1_json_command ########################################//
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
                mainController.network.stLedStrip2Data.power = true;
            }
            else
            {
                mainController.network.stLedStrip2Data.power = false;
            }
        }

        // ======== Brightness ======== //
        JsonVariant brightness = mainController.network.doc["brightness"]; 
        if(!brightness.isNull())
        {
            mainController.network.stLedStrip2Data.brightness = brightness.as<uint16_t>();
        }

        // ======== White Value ======== //
        JsonVariant white_value = mainController.network.doc["white_value"]; 
        if(!white_value.isNull())
        {
            mainController.network.stLedStrip2Data.cw = white_value.as<uint8_t>();
        }

        // ======== Color ======== //
        JsonVariant color = mainController.network.doc["color"]; 
        if(!color.isNull())
        {
            // ==== Red ==== //
            JsonVariant r = mainController.network.doc["color"]["r"]; 
            if(!r.isNull())
            {
                mainController.network.stLedStrip2Data.red = r.as<uint8_t>();
            }
            // ==== Green ==== //
            JsonVariant g = mainController.network.doc["color"]["g"]; 
            if(!g.isNull())
            {
                mainController.network.stLedStrip2Data.green = g.as<uint8_t>();
            }
            // ==== Blue ==== //
            JsonVariant b = mainController.network.doc["color"]["b"]; 
            if(!b.isNull())
            {
                mainController.network.stLedStrip2Data.blue = b.as<uint8_t>();
            }
        }

        // ======== Effect ======== //
        JsonVariant effect = mainController.network.doc["effect"]; 
        if(!effect.isNull())
        {
            mainController.network.stLedStrip2Data.effect = StringToLEDEffect(effect.as<String>());
        }

        // Send message back to mqtt state topic
        mainController.network.mqttClient.publish(mqtt_strip2_json_state, memMessage);
    }

}


