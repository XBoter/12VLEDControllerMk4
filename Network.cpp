#include "Network.h"

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
#define DEV_CONTROLLER_MQTT_PATHS
#include <secrets_mqtt_paths.h>

using namespace LedControllerSoftwareMk5;

Network::Network(){

};

// For compiler
void mqttCallback(char *topic, byte *payload, unsigned int length);

void Network::Init()
{
    mqttClient.setClient(wifiMqtt);
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    Serial.println("Network initialized");
};

void Network::Run()
{
    mqttClient.loop();

    // Wifi
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

    //Mqtt
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
};

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

void Network::HandleMqtt()
{
    switch (mqttState)
    {
    case MQTTState::StartMqtt:
        if (mqttClient.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD))
        {

            /*
            mqttClient.subscribe(mqtt_command_garage_door_up);   //Command for Garage Door Up
            mqttClient.subscribe(mqtt_command_garage_door_stop); //Command for Garage Door Stop
            mqttClient.subscribe(mqtt_command_garage_door_down); //Command for Garage Door Down
            */   

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

void mqttCallback(char *topic, byte *payload, unsigned int length)
{

    //-- Get Message and add terminator
    char message[length + 1];
    for (int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';

    //######################################## Specific ########################################//
    /*
    //------------------- Parameter [mqtt_garage_door_up] -------------------//
    if (String(mqtt_command_garage_door_up).equals(topic))
    {
        uint8_t data = atoi(message);
        //Boundary Check
        if (data >= 0 and data <= 1)
        {
            mainController.networkController.commandGarageUp = (bool)data;
            // For taster functions send only on 1 an off command
            if (data)
            {
                mainController.networkController.mqttClient.publish(mqtt_command_garage_door_up, "0");
                mainController.networkController.mqttClient.publish(mqtt_state_garage_door_up, "0");
            }
        }
    }
    */
}
