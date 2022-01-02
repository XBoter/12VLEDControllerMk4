#include "Webserver.h"

/**
 * @brief ESP Webserver instance which we use to display the webpages
 * 
 */
ESP8266WebServer server(80);
WebSocketsServer webSocketServer(81);

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
void Webserver::setReference(Filesystem *filesystem,
                             Helper *helper,
                             Network *network)
{
    this->filesystem = filesystem;
    this->helper = helper;
    this->network = network;
};

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

        Serial.println(F("Webserver initialized"));
        init = true;

        webSocketServer.begin();
        webSocketServer.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
                                { this->WebSocketEvent(num, type, payload, length); });
    }
    return init;
};

/**
 * Runs the Webserver component
 */
void Webserver::Run()
{
    if (!init)
    {
        return;
    }

    webSocketServer.loop();

    unsigned long curMillis = millis();

    if (this->filesystem->isConfigurationDataReady() && !this->mDNSInit)
    {
        // We use again the configured mqtt client name for the mDNS
        if (MDNS.begin(filesystem->getConfigurationData().mqttClientName.c_str()))
        {
            Serial.println("MDNS responder started");
            MDNS.addService("http", "tcp", 80);
            MDNS.addService("ws", "tcp", 81);
        }
        else
        {
            Serial.println("Error setting up MDNS responder!");
        }
        this->mDNSInit = true;
    }

    // == Check flash button press => Change to configuration mode
    if (digitalRead(0) == 0 && this->isInNormalMode && !this->changeToConfigurationModeRequest)
    {
        if (curMillis - this->prevMillisConfigurationMode >= this->timeoutConfigurationMode)
        {
            RequestChangeToConfigurationMode();
            this->prevMillisConfigurationMode = curMillis;
        }
    }
    else
    {
        this->prevMillisConfigurationMode = curMillis;
    }

    // == Check flash button press => Change to normal mode
    if (digitalRead(0) == 0 && this->isInConfigurationMode && !this->changeToNormalModeRequest)
    {
        if (curMillis - this->prevMillisNormalMode >= this->timeoutNormalMode)
        {
            RequestChangeToNormalMode();
            this->prevMillisNormalMode = curMillis;
        }
    }
    else
    {
        this->prevMillisNormalMode = curMillis;
    }

    // == Check first configuration
    if (this->filesystem->isConfigurationDataReady() && this->isInNormalMode && !this->changeToConfigurationModeRequest)
    {
        if (!this->filesystem->getConfigurationData().isConfigured)
        {
            RequestChangeToConfigurationMode();
        }
    }

    // == ModeHandler
    if (this->changeToConfigurationModeRequest)
    {
        this->shutdownConfigurationMode = true;
        this->shutdownNormalMode = true;
        if (this->configurationModeState == WebserverConfigurationModeState::IdleConfigurationMode)
        {
            this->configurationModeSubState = WebserverConfigurationModeSubState::BeginWebserver;
            this->isInConfigurationMode = true;
            this->isInNormalMode = false;
            this->shutdownConfigurationMode = false;
            this->changeToConfigurationModeRequest = false;
        }
    }
    else if (this->changeToNormalModeRequest)
    {
        this->shutdownConfigurationMode = true;
        this->shutdownNormalMode = true;
        if (this->normalModeState == WebserverNormalModeState::IdleNormalMode)
        {
            this->normalModeSubState = WebserverNormalModeSubState::BeginWebserver;
            this->isInConfigurationMode = false;
            this->isInNormalMode = true;
            this->shutdownNormalMode = false;
            this->changeToNormalModeRequest = false;
        }
    }
    ConfigurationModeHandler(this->shutdownConfigurationMode);
    NormalModeHandler(this->shutdownNormalMode);
};

/**
 * @brief Handles the webserver in configuration mode
 * 
 * @param shutdown If true shuts down the configuration mode
 */
void Webserver::ConfigurationModeHandler(bool shutdown)
{

    WiFiClient client;
    bool ret = false;

    switch (this->configurationModeState)
    {
        // ================================ StartConfigurationMode ================================ //
    case WebserverConfigurationModeState::StartConfigurationMode:
        if (!shutdown)
        {

            this->helper->TopSpacerPrint();
            this->helper->InsertPrint();
            Serial.println(F("Webserver changing to configuration mode"));
            this->helper->BottomSpacerPrint();

            this->network->RequestChangeToAccessPointMode();

            this->configurationData = this->filesystem->getConfigurationData();

            this->configurationModeState = WebserverConfigurationModeState::RunConfigurationMode;
        }
        else
        {
            this->configurationModeState = WebserverConfigurationModeState::IdleConfigurationMode;
        }
        break;

        // ================================ RunConfigurationMode ================================ //
    case WebserverConfigurationModeState::RunConfigurationMode:
        this->blinkOnBoardLED(500);

        if (this->network->isInAccessPointMode())
        {
            switch (this->configurationModeSubState)
            {

                // ================================ BeginWebserver ================================ //
            case WebserverConfigurationModeSubState::BeginWebserver:
                if (shutdown)
                {
                    this->configurationModeSubState = WebserverConfigurationModeSubState::StopWebServer;
                }
                else
                {
                    server.on("/", [this]()
                              { this->ConfigurationWebpage(); });
                    server.on("/submitted", [this]()
                              { this->ConfigurationWebpageSubmitted(); });
                    server.onNotFound([this]()
                                      { this->ConfigurationNotFoundWebpage(); });
                    server.begin();
                    this->configurationModeSubState = WebserverConfigurationModeSubState::HandleClients;
                }
                break;

                // ================================ HandleClients ================================ //
            case WebserverConfigurationModeSubState::HandleClients:
                if (shutdown)
                {
                    this->configurationModeSubState = WebserverConfigurationModeSubState::StopWebServer;
                }
                else
                {
                    server.handleClient();
                    client = server.client();
                }
                break;

                // ================================ StopWebServer ================================ //
            case WebserverConfigurationModeSubState::StopWebServer:

                this->filesystem->resetConfiguration();
                this->filesystem->saveConfiguration(this->configurationData);

                this->configurationModeState = WebserverConfigurationModeState::ShutdownConfigurationMode;
                break;
            }
        }
        else
        {
            if (shutdown)
            {
                this->configurationModeState = WebserverConfigurationModeState::ShutdownConfigurationMode;
            }
        }

        break;

        // ================================ ShutdownConfigurationMode ================================ //
    case WebserverConfigurationModeState::ShutdownConfigurationMode:
        this->turnOffOnBoardLED();

        this->configurationModeState = WebserverConfigurationModeState::IdleConfigurationMode;
        break;

        // ================================ IdleConfigurationMode ================================ //
    case WebserverConfigurationModeState::IdleConfigurationMode:
        if (!shutdown)
        {
            this->configurationModeState = WebserverConfigurationModeState::StartConfigurationMode;
        }
        break;

    default:
        Serial.println(F("Webserver unknown configuration mode state!"));
        break;
    }
}

/**
 * @brief Handles the webserver in normal mode
 * 
 * @param shutdown If true shuts down the normal mode
 */
void Webserver::NormalModeHandler(bool shutdown)
{

    WiFiClient client;
    bool ret = false;

    switch (this->normalModeState)
    {
        // ================================ StartNormalMode ================================ //
    case WebserverNormalModeState::StartNormalMode:
        if (!shutdown)
        {

            this->helper->TopSpacerPrint();
            this->helper->InsertPrint();
            Serial.println(F("Webserver changing to normal mode"));
            this->helper->BottomSpacerPrint();

            this->network->RequestChangeToWiFiMode();

            this->normalModeState = WebserverNormalModeState::RunNormalMode;
        }
        else
        {
            this->normalModeState = WebserverNormalModeState::IdleNormalMode;
        }
        break;

        // ================================ RunNormalMode ================================ //
    case WebserverNormalModeState::RunNormalMode:
        this->turnOffOnBoardLED();

        if (this->network->isInWiFiMode())
        {
            switch (this->normalModeSubState)
            {

                // ================================ BeginWebserver ================================ //
            case WebserverNormalModeSubState::BeginWebserver:
                if (shutdown)
                {
                    this->normalModeSubState = WebserverNormalModeSubState::StopWebServer;
                }
                else
                {
                    server.on("/", [this]()
                              { this->NormalMainWebpage(); });
                    server.on("/event", [this]()
                              { this->NormalMainWebpageEvent(); });
                    server.on("/settings", [this]()
                              { this->NormalSettingsWebpage(); });
                    server.on("/settings/event", [this]()
                              { this->NormalSettingsWebpageEvent(); });
                    server.onNotFound([this]()
                                      { this->NormalNotFoundWebpage(); });
                    server.begin();
                    this->normalModeSubState = WebserverNormalModeSubState::HandleClients;
                }
                break;

                // ================================ HandleClients ================================ //
            case WebserverNormalModeSubState::HandleClients:
                if (shutdown)
                {
                    this->normalModeSubState = WebserverNormalModeSubState::StopWebServer;
                }
                else
                {
                    server.handleClient();
                    client = server.client();
                }
                break;

                // ================================ StopWebServer ================================ //
            case WebserverNormalModeSubState::StopWebServer:
                server.stop();
                this->normalModeState = WebserverNormalModeState::ShutdownNormalMode;
                break;
            }
        }
        else
        {
            if (shutdown)
            {
                this->normalModeState = WebserverNormalModeState::ShutdownNormalMode;
            }
        }
        break;

        // ================================ ShutdownNormalMode ================================ //
    case WebserverNormalModeState::ShutdownNormalMode:

        this->normalModeState = WebserverNormalModeState::IdleNormalMode;
        break;

        // ================================ IdleNormalMode ================================ //
    case WebserverNormalModeState::IdleNormalMode:
        if (!shutdown)
        {
            this->normalModeState = WebserverNormalModeState::StartNormalMode;
        }
        break;

    default:
        Serial.println(F("Webserver unknown normal mode state!"));
        break;
    }
}

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
 * @brief Displays the configuration webpage
 * 
 */
void Webserver::ConfigurationWebpage()
{
    server.send(200, "text/html", ConfigurationPage);
};

/**
 * @brief Reads the arguments from the configuration webpage and displays the configuration submitted webpage
 * 
 */
void Webserver::ConfigurationWebpageSubmitted()
{

    if (server.hasArg("wifiSSID"))
    {
        this->configurationData.wifiSSID = server.arg("wifiSSID");
    }
    if (server.hasArg("wifiPassword"))
    {
        this->configurationData.wifiPassword = server.arg("wifiPassword");
    }
    if (server.hasArg("mqttBrokerIpAddress"))
    {
        this->configurationData.mqttBrokerIpAddress = server.arg("mqttBrokerIpAddress");
    }
    if (server.hasArg("mqttBrokerUsername"))
    {
        this->configurationData.mqttBrokerUsername = server.arg("mqttBrokerUsername");
    }
    if (server.hasArg("mqttBrokerPassword"))
    {
        this->configurationData.mqttBrokerPassword = server.arg("mqttBrokerPassword");
    }
    if (server.hasArg("mqttBrokerPort"))
    {
        this->configurationData.mqttBrokerPort =
            strtol(server.arg("mqttBrokerPort").c_str(), NULL, 0);
    }
    if (server.hasArg("mqttClientName"))
    {
        this->configurationData.mqttClientName = server.arg("mqttClientName");
    }
    this->configurationData.isConfigured = true;

    server.send(200, "text/html", SubmittedConfigurationPage);

    this->helper->TopSpacerPrint();
    this->helper->InsertPrint();
    Serial.println(F("Webserver configuration submitted"));
    this->helper->BottomSpacerPrint();

    // After submitting change back to normal mode
    this->RequestChangeToNormalMode();
};

/**
 * @brief Sends a 404 error message to the client with a "Configuration page not found" message
 * 
 */
void Webserver::ConfigurationNotFoundWebpage()
{
    String message = "Configuration page not found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
}

/**
 * @brief Displays the main page in normal mode
 * 
 */
void Webserver::NormalMainWebpage()
{
    server.send(200, "text/html", MainPage);
};

/**
 * @brief Gets called if there is a event on the main page
 * 
 */
void Webserver::NormalMainWebpageEvent(){

};

/**
 * @brief Displays the settings page in normal mode
 * 
 */
void Webserver::NormalSettingsWebpage()
{
    server.send(200, "text/html", SettingsPage);
};

/**
 * @brief Gets called if there is a event on the settings page
 * 
 */
void Webserver::NormalSettingsWebpageEvent(){

};

/**
 * @brief Sends a 404 error message to the client with a "Normal page not found" message
 * 
 */
void Webserver::NormalNotFoundWebpage()
{
    String message = "Normal page not found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
}

bool Webserver::getConfigurationMode()
{
    return this->isInConfigurationMode;
}

void Webserver::RequestChangeToConfigurationMode()
{
    if (!this->changeToNormalModeRequest)
    {
        this->changeToConfigurationModeRequest = true;
    }
}

bool Webserver::getNormalMode()
{
    return this->isInNormalMode;
}

void Webserver::RequestChangeToNormalMode()
{
    if (!this->changeToConfigurationModeRequest)
    {
        this->changeToNormalModeRequest = true;
    }
}

void Webserver::WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

    char message[length + 1];
    for (unsigned int i = 0; i < length; i++)
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';

    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("Client [%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocketServer.remoteIP(num);
        Serial.printf("Client [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocketServer.sendTXT(num, String("Connected to " + this->filesystem->getConfigurationData().mqttClientName).c_str());

        // ======== Check for valid endpoint's ======== //
        if (String("/settings").equals(message))
        {
            if (this->filesystem->isSettingsDataReady())
            {
                Serial.printf("Client [%u] Sending initial settings data", num);
                for (int i = 0; i < STRIP_COUNT; i++)
                {
                    for (int j = 0; j < CHANNEL_COUNT; j++)
                    {
                        String msg = "StripConfig#" + String(i + 1) + "#" + String(j + 1) + "#" + String(this->helper->LEDOutputTypeToUint8(this->filesystem->getSettingData().stripChannelOutputs[i][j]));
                        webSocketServer.sendTXT(num, msg);
                    }
                }
            }
        }
        else if (String("/main").equals(message))
        {
            if (this->filesystem->isLEDStateDataReady())
            {
            }
        }
    }
    break;
    case WStype_TEXT:
    {
        Serial.printf("Client [%u] Text: %s\n", num, payload);
        String data[MAX_DATA]{""};
        uint8_t dataCounter = 0;
        char *pch;
        pch = strtok(message, "#");
        while (pch != NULL)
        {
            if (dataCounter < MAX_DATA)
            {
                data[dataCounter] = pch;
                dataCounter++;
            }
            else
            {
                break;
            }
            pch = strtok(NULL, "#");
        }

        Serial.println("Type    : " + data[0]);
        Serial.println("Data 1  : " + data[1]);
        Serial.println("Data 2  : " + data[2]);
        Serial.println("Data 3  : " + data[3]);

        if (data[0].equals("StripConfig"))
        {
            uint8_t stripNumber = data[1].toInt();
            uint8_t channelID = data[2].toInt();
            uint8_t outputType = data[3].toInt();

            if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
            {
                if (channelID >= 1 && channelID <= CHANNEL_COUNT)
                {
                    SettingsData settingsData = this->filesystem->getSettingData();
                    settingsData.stripChannelOutputs[stripNumber - 1][channelID - 1] = this->helper->Uint8ToLEDOutputType(outputType);
                    String msg = "StripConfig#" + String(stripNumber) + "#" + String(channelID) + "#" + String(outputType);
                    // We Broadcast the new data to all connected clients
                    webSocketServer.broadcastTXT(msg);
                    this->filesystem->saveSettings(settingsData);
                }
            }
        }
    }
    break;

    case WStype_BIN:
    {
        Serial.printf("Client [%u] Binary length: %u\n", num, length);
        hexdump(payload, length);
    }
    break;
    }
}