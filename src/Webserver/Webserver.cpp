#include "Webserver.h"

/**
 * @brief ESP Webserver instance which we use to display the webpages
 * 
 */
ESP8266WebServer server(80);

/**
 * @brief Construct a new Webserver:: Webserver object
 * 
 */
Webserver::Webserver(){

};

/**
 * @brief Sets the needed refernce for the webserver
 * 
 */
void Webserver::setReference(){};

/**
 * @brief Blinks the onBoard LED in the given interval
 * 
 * @param interval The blink intervall in milliseconds
 */
void Webserver::blinkOnBoardLED(uint16_t interval)
{
    if (millis() >= (this->prevMillisBlinkOnBoardLED + interval))
    {
        this->prevMillisBlinkOnBoardLED = millis();
        this->onboardLEDState = !this->onboardLEDState;
        if (this->onboardLEDState)
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }
}

/**
 * @brief Turns of the onBoard LED
 * 
 */
void Webserver::turnOffOnBoardLED()
{
    this->onboardLEDState = false;
    digitalWrite(LED_BUILTIN, HIGH);
}

/**
 * @brief Initializes the webserver component
 * 
 * @return True if the initialization was successful
 */
bool Webserver::Init()
{
    if (!init)
    {
        // == Configure Flash button on the nodemcu as reset putton
        pinMode(0, INPUT_PULLUP);

        // == Configure the onBoard LED
        pinMode(LED_BUILTIN, OUTPUT);
        this->turnOffOnBoardLED();

        init = true;
    }
    return init;
};

/**
 * Runs the Configuration component
 */
void Webserver::Run()
{
    if (!init)
    {
        return;
    }

    bool isLedDriverConfigureMode = false;
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
            // ================ CONFIGURATION PROCEDURE ================ //
            switch (state)
            {
            case 0:
                Serial.println("");
                Serial.println("LED Controller Mk4 in configuration mode");
                state++;
                break;

                // Save that we are not configured or doing a reset
            case 1:
                data.isConfigured = false;
                isLedDriverConfigureMode = ledDriver->ConfigureMode();
                if (isLedDriverConfigureMode)
                {
                    state++;
                }
                break;

                // Stop wifi connection
            case 2:
                if (WiFi.status() == WL_CONNECTED)
                {
                    WiFi.disconnect();
                }
                else
                {
                    state++;
                }
                break;

                // Create Access Point
            case 3:
                WiFi.mode(WIFI_AP);
                WiFi.softAPConfig(this->apIP, this->apIP, IPAddress(255, 255, 255, 0));
                WiFi.softAP("LED Controller Mk4", "", 0, 0, 1);
                state++;
                break;

                // Start webserver
            case 4:
                server.on("/", [this]()
                          { this->inputForm(); });
                server.on("/final", [this]()
                          { this->inputFormFilled(); });
                server.begin();
                Serial.println("Waiting for user configuration");
                state++;
                break;

                // Wait for user configuration
            case 5:
                server.handleClient();
                prevMillisAPShutdown = curMillis;
                break;

                // Wait before shuting down AP
            case 6:
                if (curMillis - prevMillisAPShutdown >= timeoutAPShutdown)
                {
                    Serial.println("User finished configuration");
                    state = 99;
                }
                break;

                // Reset everything and we are good to go
            case 99:
                WiFi.softAPdisconnect(true);
                ledOn = false;
                digitalWrite(LED_BUILTIN, HIGH);
                resetOrNotConfigured = false;
                data.isConfigured = true;
                resetConfig();
                saveConfig();
                isFinished = true;
                Serial.println("LED Controller Mk4 finished configuration");
                Serial.println("");
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

void Webserver::ConfigurationModeHandler()
{
}

void Webserver::NormalModeHandler()
{
}

void Webserver::inputForm()
{
    server.send(200, "text/html", ConfigurationPage);
};

/*
    Gets called on access to url 192.168.4.1/final
*/
void Webserver::inputFormFilled()
{

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
        data.mqttBrokerPort =
            strtol(server.arg("mqttBrokerPort").c_str(), NULL, 0);
    }
    if (server.hasArg("mqttClientName"))
    {
        data.mqttClientName = server.arg("mqttClientName");
    }
    data.isConfigured = false;

    server.send(200, "text/html", SubmittedConfigurationPage);
    // Go to next state
    state++;
};

bool Webserver::getConfigurationMode()
{
    return this->configurationMode;
}

void Webserver::RequestChangeToConfigurationMode()
{
    this->changeToConfigurationModeRequest = true;
}

bool Webserver::getNormalMode()
{
    return this->normalMode;
}

void Webserver::RequestChangeToNormalMode()
{
    this->changeToNormalModeRequest = true;
}