#include "../include/Configuration.h"
#include "../Main.h"

// Webserver instance and handle
ESP8266WebServer server(80);
/*
    Gets called on access to url 192.168.4.1/
*/
void inputForm()
{
    ConfiguredData data = mainController.configuration.getData();

    String WEP_PAGE = "<!DOCTYPE html>";
    WEP_PAGE += "<html>";
    WEP_PAGE += "<head>";
    WEP_PAGE += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=2.0, user-scalable=no'>";
    WEP_PAGE += "<style>";
    WEP_PAGE += "body {background-color: #616161; width: 100vw; height: 100vh;}";
    WEP_PAGE += "* {color: #ffffff; font-family: Arial, Helvetica, sans-serif; text-align: center; margin: auto; font-size: 20px;}";
    WEP_PAGE += "#main {margin:0 auto;}";
    WEP_PAGE += "input {color: #000000;}";
    WEP_PAGE += "</style>";
    WEP_PAGE += "</head>";
    WEP_PAGE += "<body>";
    WEP_PAGE += "<div id='main'>";
    WEP_PAGE += "<h1 style='font-size: 30px; padding: 10px;'>LED Controller Configuration</h1>";
    WEP_PAGE += "<form action='/final' method='post'>";
    WEP_PAGE += "<label>WiFi SSID               :</label>";
    WEP_PAGE += "<input type='text' name='wifiSSID'><br><br>";
    WEP_PAGE += "<label>WiFi Password           :</label>";
    WEP_PAGE += "<input type='text' name='wifiPassword'><br><br>";
    WEP_PAGE += "<label>MQTT Broker IP Address  :</label>";
    WEP_PAGE += "<input type='text' name='mqttBrokerIpAddress'><br><br>";
    WEP_PAGE += "<label>MQTT Broker Port        :</label>";
    WEP_PAGE += "<input type='number' name='mqttBrokerPort'><br><br>";
    WEP_PAGE += "<label>MQTT Broker Username    :</label>";
    WEP_PAGE += "<input type='text' name='mqttBrokerUsername'><br><br>";
    WEP_PAGE += "<label>MQTT Broker Password    :</label>";
    WEP_PAGE += "<input type='text' name='mqttBrokerPassword'><br><br>";
    WEP_PAGE += "<label>MQTT Client Name        :</label>";
    WEP_PAGE += "<input type='text' name='mqttClientName'><br><br>";
    WEP_PAGE += "<input type='submit' value='Submit'>";
    WEP_PAGE += "</form>";
    WEP_PAGE += "</div>";
    WEP_PAGE += "</body>";
    WEP_PAGE += "</html>";

    server.send(200, "text/html", WEP_PAGE);
};

/*
    Gets called on access to url 192.168.4.1/final
*/
void inputFormFilled()
{

    String WEP_PAGE = "<!DOCTYPE html>";
    WEP_PAGE += "<html>";
    WEP_PAGE += "<head>";
    WEP_PAGE += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=2.0, user-scalable=no'>";
    WEP_PAGE += "<style>";
    WEP_PAGE += "body {background-color: #616161; width: 100vw; height: 100vh;}";
    WEP_PAGE += "* {color: #ffffff; font-family: Arial, Helvetica, sans-serif; text-align: center; margin: auto; font-size: 20px;}";
    WEP_PAGE += "#main {margin:0 auto;}";
    WEP_PAGE += "input {color: #000000;}";
    WEP_PAGE += "</style>";
    WEP_PAGE += "</head>";
    WEP_PAGE += "<body>";
    WEP_PAGE += "<div id='main'>";
    WEP_PAGE += "<h1 style='font-size: 30px; padding: 10px;'>Form Submitted!</h1>";
    WEP_PAGE += "</div>";
    WEP_PAGE += "</body>";
    WEP_PAGE += "</html>";

    ConfiguredData data;

    if (server.hasArg("wifiSSID"))
    {
        data.wifiSSID = server.arg("wifiSSID");
    }
    if (server.hasArg("wifiPassword"))
    {
        data.wifiPassword = server.arg("wifiPassword");
    }
    if (server.hasArg("mqttBrokerIpAddress"))
    {
        data.mqttBrokerIpAddress = server.arg("mqttBrokerIpAddress");
    }
    if (server.hasArg("mqttBrokerUsername"))
    {
        data.mqttBrokerUsername = server.arg("mqttBrokerUsername");
    }
    if (server.hasArg("mqttBrokerPassword"))
    {
        data.mqttBrokerPassword = server.arg("mqttBrokerPassword");
    }
    if (server.hasArg("mqttBrokerPort"))
    {
        data.mqttBrokerPort = strtol(server.arg("mqttBrokerPort").c_str(), NULL, 0);
    }
    if (server.hasArg("mqttClientName"))
    {
        data.mqttClientName = server.arg("mqttClientName");
    }
    data.isConfigured = false;

    mainController.configuration.setData(data);
    server.send(200, "text/html", WEP_PAGE);
    mainController.configuration.formSubmitFinished();
};

/**
 * Empty constructor
 */
Configuration::Configuration(){

};

/**
 * Does init stuff for the Configuration component
 * 
 * @return True if successfull, false if not 
 */
bool Configuration::Init()
{

    // Configure Flash button on the nodemcu as reset putton
    pinMode(0, INPUT_PULLUP);

    // Enable blink led
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(LED_BUILTIN, OUTPUT);

    EEPROM.begin(512);
    EEPROM.get(configDataAddr, data);

    // DEBUG get current saved config data
    Serial.println("#==== EEPROM Content ====#");
    Serial.println("WiFi Password   : " + String(data.wifiPassword));
    Serial.println("WiFi SSID       : " + String(data.wifiSSID));
    Serial.println("Broker Address  : " + String(data.mqttBrokerIpAddress));
    Serial.println("Broker User     : " + String(data.mqttBrokerUsername));
    Serial.println("Broker Password : " + String(data.mqttBrokerPassword));
    Serial.println("Broker Port     : " + String(data.mqttBrokerPort));
    Serial.println("Client Name     : " + String(data.mqttClientName));
    Serial.println("Is Configured   : " + String(data.isConfigured));
    Serial.println("");

    // Check if controller is already configured
    if (!data.isConfigured)
    {
        resetOrNotConfigured = true;
    }
    else
    {
        // All okay we can procedure with the rest
        isFinished = true;
    }

    return true;
};

/**
 * Runs the Configuration component
 */
void Configuration::Run()
{

    unsigned long curMillis = millis();
    // Check if button is pressed longer then 10 sec
    if (digitalRead(0) == 0)
    {
        if (curMillis - prevMillisReset >= timeoutRest)
        {
            isFinished = false;
            resetOrNotConfigured = true;
        }

        if (!resetOrNotConfigured)
        {
            prevMillisResetBlink = curMillis;
        }
    }
    else
    {
        // Wait for button release or if not configured start configuration
        if (resetOrNotConfigured)
        {

            //Serial.println("State : " + String(state));
            // ================ CONFIGURATION PROCEDURE ================ //
            switch (state)
            {
                // Save that we are not configured or doing a reset
            case 0:
                data.isConfigured = false;
                saveConfigToEEPROM();
                state++;
                break;

                // Stop wifi connection
            case 1:
                if (WiFi.status() == WL_CONNECTED)
                {
                    WiFi.disconnect();
                }
                else
                {
                    state++;
                }
                break;

                // Create hotspot default IP Address of ESP 192.168.4.1.
            case 2:
                WiFi.softAP("LED Controller Mk4");
                state++;
                break;

                // Start webserver
            case 3:
                server.on("/", inputForm);
                server.on("/final", inputFormFilled);
                server.begin();
                state++;
                break;

                // Wait for user configuration
            case 4:
                server.handleClient();
                break;

                // Save user input data
            case 5:
                saveConfigToEEPROM();
                state = 99;
                break;

                // Reset everything and we are good to go
            case 99:
                WiFi.softAPdisconnect(true);
                ledOn = false;
                digitalWrite(LED_BUILTIN, HIGH);
                resetOrNotConfigured = false;
                data.isConfigured = true;
                saveConfigToEEPROM();
                isFinished = true;
                state = 0;
                break;
            }
        }

        prevMillisReset = curMillis;
    }

    // Blink onboard led as long as we are in configuration
    if (resetOrNotConfigured == true)
    {
        if (curMillis - prevMillisResetBlink >= timeoutResetBlink)
        {

            if (ledOn)
            {
                digitalWrite(LED_BUILTIN, LOW);
            }
            else
            {
                digitalWrite(LED_BUILTIN, HIGH);
            }
            ledOn = !ledOn;

            prevMillisResetBlink = curMillis;
        }
    }
};

/**
 * Returns the current loaded or configured config
 * 
 * @return The ConfiguredData 
 */
ConfiguredData Configuration::getData()
{
    return data;
};

/**
 * Saves the configured data to the EEPROM
 */
void Configuration::saveConfigToEEPROM()
{
    Serial.println("");
    Serial.println("EEPROM commit / save action!");
    Serial.println("");
    EEPROM.put(configDataAddr, data);
    EEPROM.commit();
};

/**
 * Handles webserver root
 */
void Configuration::setData(ConfiguredData data)
{
    this->data = data;
};

/**
 * Handles webserver root
 */
void Configuration::formSubmitFinished()
{
    state++;
};