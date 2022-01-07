#include "Webserver.h"

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
                             Network *network,
                             Parameterhandler *parameterhandler)
{
    this->filesystem = filesystem;
    this->helper = helper;
    this->network = network;
    this->parameterhandler = parameterhandler;
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
        Serial.println(F("Webserver initialized"));

        this->asyncWebServer.begin();

        init = true;
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

    unsigned long curMillis = millis();

    // MDNS update
    MDNS.update();

    // Cleanup Websocket connections
    if (curMillis - this->prevMillisWebsocketCleanup >= this->timeoutWebsocketCleanup)
    {

        this->asyncWebSocketMain.cleanupClients();
        this->asyncWebSocketSettings.cleanupClients();

        this->prevMillisWebsocketCleanup = curMillis;
    }

    // == Check flash button press => Change to configuration mode
    if (digitalRead(0) == 0 &&
        this->isInNormalMode &&
        !this->isInConfigurationMode &&
        !this->changeToConfigurationModeRequest &&
        !this->changeToNormalModeRequest)
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
    if (digitalRead(0) == 0 &&
        this->isInConfigurationMode &&
        !this->isInNormalMode &&
        !this->changeToConfigurationModeRequest &&
        !this->changeToNormalModeRequest)
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
    if (this->filesystem->isConfigurationDataReady() &&
        this->isInNormalMode &&
        !this->isInConfigurationMode &&
        !this->changeToConfigurationModeRequest &&
        !this->changeToNormalModeRequest)
    {
        if (!this->filesystem->getConfigurationData().isConfigured &&
            !this->filesystem->getConfigurationData().isStandaloneMode &&
            !this->filesystem->getConfigurationData().isFullyConfigured)
        {
            RequestChangeToConfigurationMode();
        }
    }

    // == ModeHandler
    if (this->changeToConfigurationModeRequest && !this->changeToNormalModeRequest)
    {
        this->shutdownConfigurationMode = true;
        this->shutdownNormalMode = true;
        if (this->normalModeState == WebserverNormalModeState::IdleNormalMode)
        {
            this->webserverResetState = 0;
            this->configurationModeSubState = WebserverConfigurationModeSubState::BeginWebserver;
            this->isInConfigurationMode = true;
            this->isInNormalMode = false;
            this->shutdownConfigurationMode = false;
            this->changeToConfigurationModeRequest = false;
        }
    }
    if (this->changeToNormalModeRequest && !this->changeToConfigurationModeRequest)
    {
        this->shutdownConfigurationMode = true;
        this->shutdownNormalMode = true;
        if (this->configurationModeState == WebserverConfigurationModeState::IdleConfigurationMode)
        {
            this->webserverResetState = 0;
            this->normalModeSubState = WebserverNormalModeSubState::BeginWebserver;
            this->isInConfigurationMode = false;
            this->isInNormalMode = true;
            this->shutdownNormalMode = false;
            this->changeToNormalModeRequest = false;
        }
    }
    ConfigurationModeHandler(this->shutdownConfigurationMode);
    NormalModeHandler(this->shutdownNormalMode);
    HandleMDNS();
};

void Webserver::HandleMDNS()
{
    // Inital Checks
    if (this->filesystem->isConfigurationDataReady() &&
        (this->network->isWiFiConnected() || this->network->isAccessPointReady()) &&
        (this->filesystem->getConfigurationData().MQTTClientName.length() > 0))
    {
        // ======== INIT
        if (!this->mDNSInit)
        {

            // We use again the configured mqtt client name for the mDNS
            if (MDNS.begin(this->filesystem->getConfigurationData().MQTTClientName))
            {
                this->helper->SimplePrint(F("MDNS responder started"));

                MDNS.addService("http", "tcp", 80);
                MDNS.addService("ws", "tcp", 81);
            }
            else
            {
                this->helper->SimplePrint(F("Error setting up MDNS responder!"));
            }
            this->mDNSInit = true;
        }
        // ======== RESTART
        if (this->restartMDNS && this->mDNSInit)
        {
            this->restartMDNS = false;
            this->restartStateMDNS = 0;
        }
        switch (this->restartStateMDNS)
        {
        case 0:
            this->helper->SimplePrint(F("Restarting MDNS responder"));
            this->restartStateMDNS++;
            break;
        case 1:
            if (MDNS.close())
            {
                this->restartStateMDNS++;
            }
            break;
        case 2:

            if (MDNS.begin(this->filesystem->getConfigurationData().MQTTClientName))
            {
                this->helper->SimplePrint(F("MDNS responder restarted"));

                MDNS.addService("http", "tcp", 80);
                MDNS.addService("ws", "tcp", 81);
            }
            this->restartStateMDNS++;
            break;
        default:
            break;
        }
    }
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
            this->helper->SimplePrint(F("Webserver changing to configuration mode"));

            this->network->RequestChangeToAccessPointMode();

            this->configurationModeState = WebserverConfigurationModeState::RunConfigurationMode;
        }
        else
        {
            this->configurationModeState = WebserverConfigurationModeState::IdleConfigurationMode;
        }
        break;

        // ================================ RunConfigurationMode ================================ //
    case WebserverConfigurationModeState::RunConfigurationMode:
        this->helper->blinkOnBoardLED(500);

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
                    if (this->network->isWiFiConnected() || this->network->isAccessPointReady())
                    {
                        this->indexHandle = &this->asyncWebServer.on("/", HTTP_ANY, std::bind(&Webserver::ConfigurationWebpage, this, std::placeholders::_1));
                        this->submittedHandle = &this->asyncWebServer.on("/submitted", HTTP_ANY, std::bind(&Webserver::ConfigurationWebpageSubmitted, this, std::placeholders::_1));
                        this->asyncWebServer.onNotFound(std::bind(&Webserver::ConfigurationNotFoundWebpage, this, std::placeholders::_1));

                        //this->asyncWebServer.begin();

                        this->helper->SimplePrint(F("Webserver is in configuration mode"));
                        this->configurationModeSubState = WebserverConfigurationModeSubState::HandleClients;
                    }
                }
                break;

                // ================================ HandleClients ================================ //
            case WebserverConfigurationModeSubState::HandleClients:
                if (shutdown)
                {
                    this->configurationModeSubState = WebserverConfigurationModeSubState::StopWebServer;
                }
                break;

                // ================================ StopWebServer ================================ //
            case WebserverConfigurationModeSubState::StopWebServer:
                switch (this->webserverResetState)
                {
                case 0:
                    this->helper->SimplePrint(F("Webserver shutting down configuration mode"));
                    this->webserverResetState++;
                    break;
                case 1:
                    if (this->asyncWebServer.removeHandler(this->indexHandle))
                    {
                        this->webserverResetState++;
                    }
                    break;
                case 2:
                    if (this->asyncWebServer.removeHandler(this->submittedHandle))
                    {
                        this->webserverResetState++;
                    }
                    break;
                case 3:
                    this->asyncWebServer.onNotFound(NULL);
                    this->webserverResetState++;
                    break;
                case 4:
                    //this->asyncWebServer.end();
                    this->webserverResetState++;
                default:
                    this->helper->SimplePrint(F("Webserver shut down configuration mode"));
                    this->webserverResetState = 0;
                    this->configurationModeState = WebserverConfigurationModeState::ShutdownConfigurationMode;
                    break;
                }
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
        this->helper->turnOffOnBoardLED();

        // We check here if we are going into the standalone mode (The configuration was aborted or parameter are missing)
        {
            FilesystemConfigurationData data = this->filesystem->getConfigurationData();

            if (!data.isConfigured || !data.isFullyConfigured)
            {
                data.isStandaloneMode = true;

                this->helper->SimplePrint(F("LED Controller is in standalone mode"));
            }
            else
            {
                data.isStandaloneMode = false;

                this->helper->SimplePrint(F("LED Controller is in connected mode"));
            }
            this->filesystem->saveConfigurationData(data);
        }

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
            this->helper->SimplePrint(F("Webserver changing to normal mode"));

            // Check for standalone mode
            if (!this->filesystem->getConfigurationData().isStandaloneMode)
            {
                this->network->RequestChangeToWiFiMode();
            }

            this->normalModeState = WebserverNormalModeState::RunNormalMode;
        }
        else
        {
            this->normalModeState = WebserverNormalModeState::IdleNormalMode;
        }
        break;

        // ================================ RunNormalMode ================================ //
    case WebserverNormalModeState::RunNormalMode:
        this->helper->turnOffOnBoardLED();

        if (this->network->isInWiFiMode() || this->filesystem->getConfigurationData().isStandaloneMode)
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
                    if (this->network->isWiFiConnected() || this->network->isAccessPointReady())
                    {
                        this->indexHandle = &this->asyncWebServer.on("/", HTTP_ANY, std::bind(&Webserver::NormalMainWebpage, this, std::placeholders::_1));
                        this->settingsHandle = &this->asyncWebServer.on("/settings", HTTP_ANY, std::bind(&Webserver::NormalSettingsWebpage, this, std::placeholders::_1));
                        this->asyncWebServer.onNotFound(std::bind(&Webserver::NormalNotFoundWebpage, this, std::placeholders::_1));

                        this->asyncWebSocketMain.onEvent(std::bind(&Webserver::WebSocketEventMain, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
                        this->asyncWebSocketSettings.onEvent(std::bind(&Webserver::WebSocketEventSettings, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

                        this->asyncWebServer.addHandler(&this->asyncWebSocketMain);
                        this->asyncWebServer.addHandler(&this->asyncWebSocketSettings);

                        //this->asyncWebServer.begin();

                        this->helper->SimplePrint(F("Webserver is in normal mode"));
                        this->normalModeSubState = WebserverNormalModeSubState::HandleClients;
                    }
                }
                break;

                // ================================ HandleClients ================================ //
            case WebserverNormalModeSubState::HandleClients:
                if (shutdown)
                {
                    this->normalModeSubState = WebserverNormalModeSubState::StopWebServer;
                }
                break;

                // ================================ StopWebServer ================================ //
            case WebserverNormalModeSubState::StopWebServer:
                switch (this->webserverResetState)
                {
                case 0:
                    this->helper->SimplePrint(F("Webserver shutting down normal mode"));
                    this->webserverResetState++;
                case 1:
                    this->asyncWebSocketMain.closeAll();
                    this->asyncWebSocketSettings.closeAll();
                    this->webserverResetState++;
                    break;
                case 2:
                    if (this->asyncWebServer.removeHandler(this->indexHandle))
                    {
                        this->webserverResetState++;
                    }
                    break;
                case 3:
                    if (this->asyncWebServer.removeHandler(this->settingsHandle))
                    {
                        this->webserverResetState++;
                    }
                    break;
                case 4:
                    this->asyncWebServer.onNotFound(NULL);
                    this->webserverResetState++;
                    break;
                case 5:
                    //this->asyncWebServer.end();
                    this->webserverResetState++;
                default:
                    this->helper->SimplePrint(F("Webserver shut down normal mode"));
                    this->webserverResetState = 0;
                    this->normalModeState = WebserverNormalModeState::ShutdownNormalMode;
                    break;
                }
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
 * @brief Displays the configuration webpage
 * 
 */
void Webserver::ConfigurationWebpage(AsyncWebServerRequest *request)
{
    Serial.println("Webserver sending configuration page");
    request->send_P(200, "text/html", ConfigurationPage);
};

/**
 * @brief Reads the arguments from the configuration webpage and displays the configuration submitted webpage
 * 
 */
void Webserver::ConfigurationWebpageSubmitted(AsyncWebServerRequest *request)
{
    FilesystemConfigurationData data = this->filesystem->getConfigurationData();
    data.isFullyConfigured = true;

    if (request->hasArg("wifiSSID"))
    {
        data.WiFiSSID = request->arg("wifiSSID").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("wifiPassword"))
    {
        data.WiFiPassword = request->arg("wifiPassword").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("mqttBrokerIpAddress"))
    {
        data.MQTTBrokerIpAddress = request->arg("mqttBrokerIpAddress").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("mqttBrokerUsername"))
    {
        data.MQTTBrokerUsername = request->arg("mqttBrokerUsername").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("mqttBrokerPassword"))
    {
        data.MQTTBrokerPassword = request->arg("mqttBrokerPassword").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("mqttBrokerPort"))
    {
        data.MQTTBrokerPort =
            strtol(request->arg("mqttBrokerPort").c_str(), NULL, 0);
    }
    else
    {
        data.isFullyConfigured = false;
    }
    if (request->hasArg("mqttClientName"))
    {
        data.MQTTClientName = request->arg("mqttClientName").c_str();
    }
    else
    {
        data.isFullyConfigured = false;
    }
    data.isConfigured = true;

    Serial.println("Webserver sending config submitted page");
    request->send_P(200, "text/html", SubmittedConfigurationPage);
    this->filesystem->saveConfigurationData(data);
    this->parameterhandler->updateConfigurationParameter(data);

    if (!data.isFullyConfigured)
    {
        Serial.println("Webserver configuration is incomplete");
    }
    else
    {
        this->restartMDNS = true;
    }
    this->helper->SimplePrint(F("Webserver configuration submitted"));

    // After submitting change back to normal mode
    this->RequestChangeToNormalMode();
};

/**
 * @brief Sends a 404 error message to the client with a "Configuration page not found" message
 * 
 */
void Webserver::ConfigurationNotFoundWebpage(AsyncWebServerRequest *request)
{
    Serial.println("Webserver sending configuration not found message");
    request->send(404, "text/plain", "Configuration page not found");
}

/**
 * @brief Displays the main page in normal mode
 * 
 */
void Webserver::NormalMainWebpage(AsyncWebServerRequest *request)
{
    Serial.println("Webserver sending main page");
    request->send_P(200, "text/html", MainPage);
};

/**
 * @brief Displays the settings page in normal mode
 * 
 */
void Webserver::NormalSettingsWebpage(AsyncWebServerRequest *request)
{
    Serial.println("Webserver sending settings page");
    request->send_P(200, "text/html", SettingsPage);
};

/**
 * @brief Sends a 404 error message to the client with a "Normal page not found" message
 * 
 */
void Webserver::NormalNotFoundWebpage(AsyncWebServerRequest *request)
{
    Serial.println("Webserver sending normal not found message");
    request->send(404, "text/plain", "Normal page not found");
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

void Webserver::WebSocketEventMain(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

    switch (type)
    {
        // ================================ WS_EVT_DISCONNECT ================================ //
    case AwsEventType::WS_EVT_DISCONNECT:
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
        // ================================ WS_EVT_ERROR ================================ //
    case AwsEventType::WS_EVT_ERROR:
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
        break;
        // ================================ WS_EVT_PONG ================================ //
    case AwsEventType::WS_EVT_PONG:
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
        break;
        // ================================ WS_EVT_CONNECT ================================ //
    case AwsEventType::WS_EVT_CONNECT:
    {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        for (int i = 0; i < STRIP_COUNT; i++)
        {
            if (this->filesystem->isLEDStripDataReady(i))
            {
                Serial.printf("ws[%u] Sending initial led strip %u data \n", client->id(), i);

                String msg = this->BuildWebsocketMessage("Power", String(i + 1), String(this->filesystem->getLEDStripData(i).Power));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("Red", String(i + 1), String(this->filesystem->getLEDStripData(i).Red));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("Green", String(i + 1), String(this->filesystem->getLEDStripData(i).Green));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("Blue", String(i + 1), String(this->filesystem->getLEDStripData(i).Blue));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("WhiteTemperature", String(i + 1), String(this->filesystem->getLEDStripData(i).WhiteTemperature));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("WhiteTemperatureBrightness", String(i + 1), String(this->filesystem->getLEDStripData(i).WhiteTemperatureBrightness));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("ColorBrightness", String(i + 1), String(this->filesystem->getLEDStripData(i).ColorBrightness));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("Effect", String(i + 1), String(this->helper->SingleLEDEffectToUint8(this->filesystem->getLEDStripData(i).Effect)));
                server->text(client->id(), msg);
            }
            else
            {
                Serial.printf("ws[%u] Cant send initial led strip %u data because filesystem is not ready \n", client->id(), i);
            }
        }
    }
    break;
    // ================================ WS_EVT_DATA ================================ //
    case AwsEventType::WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len)
        {
            Serial.printf("ws[%s][%u] data %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT)
            {
                data[len] = 0;
                Serial.printf("%s\n", (char *)data);
            }
            else
            {
                Serial.println("We only support text");
                return;
            }

            Serial.printf("ws[%u] Text: %s\n", client->id(), (char *)data);
            String dataArray[MAX_DATA]{""};
            uint8_t dataCounter = 0;
            char *pch;
            pch = strtok((char *)data, "#");
            while (pch != NULL)
            {
                if (dataCounter < MAX_DATA)
                {
                    dataArray[dataCounter] = pch;
                    dataCounter++;
                }
                else
                {
                    break;
                }
                pch = strtok(NULL, "#");
            }

            Serial.println("Type    : " + dataArray[0]);
            Serial.println("Data 1  : " + dataArray[1]);
            Serial.println("Data 2  : " + dataArray[2]);
            Serial.println("Data 3  : " + dataArray[3]);

            // ================================ Power ================================ //
            if (dataArray[0].equals("Power"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t power = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.Power = power;
                    String msg = this->BuildWebsocketMessage("Power", String(stripNumber), String(power));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ RedValue ================================ //
            else if (dataArray[0].equals("RedValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t redValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.Red = redValue;
                    String msg = this->BuildWebsocketMessage("RedValue", String(stripNumber), String(redValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ GreenValue ================================ //
            else if (dataArray[0].equals("GreenValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t greenValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.Green = greenValue;
                    String msg = this->BuildWebsocketMessage("GreenValue", String(stripNumber), String(greenValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ BlueValue ================================ //
            else if (dataArray[0].equals("BlueValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t blueValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.Blue = blueValue;
                    String msg = this->BuildWebsocketMessage("BlueValue", String(stripNumber), String(blueValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ WhiteTemperature ================================ //
            else if (dataArray[0].equals("WhiteTemperature"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t whiteTemperature = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.WhiteTemperature = whiteTemperature;
                    String msg = this->BuildWebsocketMessage("WhiteTemperature", String(stripNumber), String(whiteTemperature));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ ColorBrightness ================================ //
            else if (dataArray[0].equals("ColorBrightness"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t colorBrightness = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.ColorBrightness = colorBrightness;
                    String msg = this->BuildWebsocketMessage("ColorBrightness", String(stripNumber), String(colorBrightness));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ WhiteTemperatureBrightness ================================ //
            else if (dataArray[0].equals("WhiteTemperatureBrightness"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t whiteTemperatureBrightness = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.WhiteTemperatureBrightness = whiteTemperatureBrightness;
                    String msg = this->BuildWebsocketMessage("WhiteTemperatureBrightness", String(stripNumber), String(whiteTemperatureBrightness));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
            // ================================ EffectValue ================================ //
            else if (dataArray[0].equals("Effect"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t effect = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStripParameter ledStripParameter = this->parameterhandler->getLEDStripParameter(stripNumber - 1);
                    ledStripParameter.Effect = this->helper->Uint8ToSingleLEDEffect(effect);
                    String msg = this->BuildWebsocketMessage("Effect", String(stripNumber), String(effect));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->parameterhandler->updateLEDStripParameter(stripNumber - 1, ledStripParameter);
                }
            }
        }
    }
    break;
    }
}

void Webserver::WebSocketEventSettings(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

    switch (type)
    {
        // ================================ WS_EVT_DISCONNECT ================================ //
    case AwsEventType::WS_EVT_DISCONNECT:
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
        // ================================ WS_EVT_ERROR ================================ //
    case AwsEventType::WS_EVT_ERROR:
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
        break;
        // ================================ WS_EVT_PONG ================================ //
    case AwsEventType::WS_EVT_PONG:
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
        break;
        // ================================ WS_EVT_CONNECT ================================ //
    case AwsEventType::WS_EVT_CONNECT:
    {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());

        for (int i = 0; i < STRIP_COUNT; i++)
        {
            if (this->filesystem->isSettingsStripDataReady(i))
            {
                Serial.printf("ws[%u] Sending initial settings strip %u data \n", client->id(), i + 1);

                for (int j = 0; j < CHANNEL_COUNT; j++)
                {
                    String msg = this->BuildWebsocketMessage("StripConfig", String(i + 1), String(j + 1), String(this->helper->LEDOutputTypeToUint8(this->filesystem->getSettingStripData(i).ChannelOutputType[j])));
                    server->text(client->id(), msg);
                }
            }
            else
            {
                Serial.printf("ws[%u] Cant send initial settings strip %u data because filesystem is not ready \n", client->id(), i + 1);
            }
        }
    }
    break;
    // ================================ WS_EVT_DATA ================================ //
    case AwsEventType::WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len)
        {
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT)
            {
                data[len] = 0;
                Serial.printf("%s\n", (char *)data);
            }
            else
            {
                Serial.println("We only support text");
                return;
            }

            Serial.printf("ws[%u] Text: %s\n", client->id(), (char *)data);
            String dataArray[MAX_DATA]{""};
            uint8_t dataCounter = 0;
            char *pch;
            pch = strtok((char *)data, "#");
            while (pch != NULL)
            {
                if (dataCounter < MAX_DATA)
                {
                    dataArray[dataCounter] = pch;
                    dataCounter++;
                }
                else
                {
                    break;
                }
                pch = strtok(NULL, "#");
            }

            Serial.println("Type    : " + dataArray[0]);
            Serial.println("Data 1  : " + dataArray[1]);
            Serial.println("Data 2  : " + dataArray[2]);
            Serial.println("Data 3  : " + dataArray[3]);

            // ================================ StripConfig ================================ //
            if (dataArray[0].equals("StripConfig"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t channelID = dataArray[2].toInt();
                uint8_t outputType = dataArray[3].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    if (channelID >= 1 && channelID <= CHANNEL_COUNT)
                    {
                        SettingsStripParameter settingsStripParameter = this->parameterhandler->getSettingsStripParameter(stripNumber - 1);
                        settingsStripParameter.ChannelOutputType[channelID - 1] = this->helper->Uint8ToLEDOutputType(outputType);
                        String msg = this->BuildWebsocketMessage("StripConfig", String(stripNumber), String(channelID), String(outputType));
                        // We Broadcast the new data to all connected clients
                        server->textAll(msg);
                        this->parameterhandler->updateSettingsStripParameter(stripNumber - 1, settingsStripParameter);
                    }
                }
            }
        }
    }
    break;
    }
}

String Webserver::BuildWebsocketMessage(String type, String data1, String data2, String data3)
{
    String msg = "";
    msg = type + "#" + data1 + "#" + data2 + "#" + data3;
    return msg;
}

String Webserver::BuildWebsocketMessage(String type, String data1, String data2)
{
    String msg = "";
    msg = BuildWebsocketMessage(type, data1, data2, "0");
    return msg;
}

String Webserver::BuildWebsocketMessage(String type, String data1)
{
    String msg = "";
    msg = BuildWebsocketMessage(type, data1, "0");
    return msg;
}
