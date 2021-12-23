#include "Configuration.h"

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
 * @param ledDriver pointer to the used ledDriver instance
 */
void Webserver::setReference(LedDriver *ledDriver)
{
    this->ledDriver = ledDriver;
};

/**
 * @brief Blinks the onBoard LED in the given interval
 * 
 */
void Webserver::blinkOnBoardLED(uint16_t interval)
{

    static unsigned long prevMillisBlinkOnBoardLED = 0;
    static bool 
    unsigned long currentMillisBlinkOnBoardLED = millis();

    digitalWrite(LED_BUILTIN, HIGH);
        pinMode(LED_BUILTIN, OUTPUT);
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
        // Configure Flash button on the nodemcu as reset putton
        pinMode(0, INPUT_PULLUP);

        // Enable blink led
        digitalWrite(LED_BUILTIN, HIGH);
        pinMode(LED_BUILTIN, OUTPUT);

        // Start LittleFS
        Serial.println("Mount LittleFS");
        if (!LittleFS.begin())
        {
            Serial.println("LittleFS mount Failed!");
            return init;
        }
        else
        {
            Serial.println("LittleFS mount OK!");
            Serial.println("");
        }

        // Create config.txt if missing
        createConfig();

        // List all found files
        listFiles();
        // Load config
        loadConfig();

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

        init = true;
    }

    return init;
};

/**
 * Runs the Configuration component
 */
void Configuration::Run()
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

void Configuration::inputForm()
{
    server.send(200, "text/html", ConfigurationPage);
};

/*
    Gets called on access to url 192.168.4.1/final
*/
void Configuration::inputFormFilled()
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

/**
 * Returns the current config data
 *
 * @return ConfiguredData
 */
ConfiguredData Configuration::getData() { return data; };

/**
 * Saves the current config to a persist data storage
 */
void Configuration::saveConfig()
{
    Serial.println(F("Saving Config"));
    Serial.println(F(""));

    File file = LittleFS.open("/config.txt", "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    // ==== wifiSSID ==== //
    if (!file.println(data.wifiSSID))
    {
        Serial.println(F("wifiSSID failed to save"));
    }

    // ==== wifiPassword ==== //
    if (!file.println(data.wifiPassword))
    {
        Serial.println(F("wifiPassword failed to save"));
    }

    // ==== mqttBrokerIpAddress ==== //
    if (!file.println(data.mqttBrokerIpAddress))
    {
        Serial.println(F("mqttBrokerIpAddress failed to save"));
    }

    // ==== mqttBrokerPort ==== //
    if (!file.println(String(data.mqttBrokerPort)))
    {
        Serial.println(F("mqttBrokerPort failed to save"));
    }

    // ==== mqttBrokerUsername ==== //
    if (!file.println(data.mqttBrokerUsername))
    {
        Serial.println(F("mqttBrokerUsername failed to save"));
    }

    // ==== mqttBrokerPassword ==== //
    if (!file.println(data.mqttBrokerPassword))
    {
        Serial.println(F("mqttBrokerPassword failed to save"));
    }

    // ==== mqttClientName ==== //
    if (!file.println(data.mqttClientName))
    {
        Serial.println(F("mqttClientName failed to save"));
    }

    // ==== isConfigured ==== //
    if (!file.println(String(data.isConfigured)))
    {
        Serial.println(F("isConfigured failed to save"));
    }

    delay(2000); // Make sure the CREATE and LASTWRITE times are different
    file.close();
};

/**
 * Loads the saved config from a persist data storage
 */
void Configuration::loadConfig()
{
    Serial.println(F("Loading Config"));
    Serial.println("");

    File file = LittleFS.open("/config.txt", "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return;
    }

    uint8_t state = 0;
    String message = "";
    char symbol = '\0';
    while (file.available())
    {
        int x = file.read();

        // Check for ascii symbol 13 => Carriage Return
        if (x == 13)
        {
            x = file.read();
            // Check for ascii symbol 10 => Line Feed
            if (x == 10)
            {
                switch (state)
                {
                    // ==== wifiSSID ==== //
                case 0:
                    data.wifiSSID = message;
                    state++;
                    break;
                    // ==== wifiPassword ==== //
                case 1:
                    data.wifiPassword = message;
                    state++;
                    break;
                    // ==== mqttBrokerIpAddress ==== //
                case 2:
                    data.mqttBrokerIpAddress = message;
                    state++;
                    break;
                    // ==== mqttBrokerPort ==== //
                case 3:
                    data.mqttBrokerPort = strtol(message.c_str(), NULL, 0);
                    state++;
                    break;
                    // ==== mqttBrokerUsername ==== //
                case 4:
                    data.mqttBrokerUsername = message;
                    state++;
                    break;
                    // ==== mqttBrokerPassword ==== //
                case 5:
                    data.mqttBrokerPassword = message;
                    state++;
                    break;
                    // ==== mqttClientName ==== //
                case 6:
                    data.mqttClientName = message;
                    state++;
                    break;
                    // ==== isConfigured ==== //
                case 7:
                    data.isConfigured = bool(message);
                    break;
                }
                message = "";
            }
        }
        else
        {
            if (x != 0)
            {
                symbol = char(x);
                message += String(symbol);
            }
        }
    }
    file.close();
};

/**
 * Lists all configs found on the persits data storage
 */
void Configuration::listFiles()
{
    Serial.println(F("Found files in root dir:"));

    Dir root = LittleFS.openDir("/");

    while (root.next())
    {
        File file = root.openFile("r");
        Serial.print(F("  FILE: "));
        Serial.print(root.fileName());
        Serial.print(F("  SIZE: "));
        Serial.println(file.size());
        file.close();
    }
};

/**
 * Lists all configs found on the persits data storage
 */
void Configuration::createConfig()
{
    Serial.println(F("Create config.txt file if missing"));
    if (!LittleFS.exists("/config.txt"))
    {
        Serial.println(F("Creating new config.txt file"));
        File file = LittleFS.open("/config.txt", "w+");
        file.close();
    }
    else
    {
        Serial.println(F("Found existing config.txt file"));
    }
};

/**
 * Resets the config file
 */
void Configuration::resetConfig()
{
    Serial.println(F("Reset config.txt file"));
    if (LittleFS.exists("/config.txt"))
    {
        File file = LittleFS.open("/config.txt", "w");
        file.close();
    }
};
