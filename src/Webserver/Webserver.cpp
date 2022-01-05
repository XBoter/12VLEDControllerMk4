#include "Webserver.h"

/**
 * @brief ESP Webserver instance which we use to display the webpages
 * 
 */
AsyncWebServer asyncWebServer(80);
AsyncWebSocket asyncWebSocketMain("/ws/main");
AsyncWebSocket asyncWebSocketSettings("/ws/Settings");

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
        // == Configure Flash button on the nodemcu as reset putton
        pinMode(0, INPUT_PULLUP);

        // == Configure the onBoard LED
        pinMode(LED_BUILTIN, OUTPUT);
        this->turnOffOnBoardLED();

        Serial.println(F("Webserver initialized"));
        init = true;

        asyncWebSocketMain.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                                   { this->WebSocketEventMain(server, client, type, arg, data, len); });
        asyncWebSocketSettings.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                                       { this->WebSocketEventSettings(server, client, type, arg, data, len); });
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
                    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                                      { this->ConfigurationWebpage(request); });
                    asyncWebServer.on("/submitted", HTTP_GET, [this](AsyncWebServerRequest *request)
                                      { this->ConfigurationWebpageSubmitted(request); });
                    asyncWebServer.onNotFound([this](AsyncWebServerRequest *request)
                                              { this->ConfigurationNotFoundWebpage(request); });
                    asyncWebServer.begin();
                    this->configurationModeSubState = WebserverConfigurationModeSubState::HandleClients;
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

                this->filesystem->resetConfiguration();
                this->filesystem->saveConfiguration(this->configurationData);

                asyncWebServer.end();
                asyncWebServer.reset();

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
                    asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                                      { this->NormalMainWebpage(request); });
                    asyncWebServer.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request)
                                      { this->NormalSettingsWebpage(request); });
                    asyncWebServer.onNotFound([this](AsyncWebServerRequest *request)
                                              { this->NormalNotFoundWebpage(request); });
                    asyncWebServer.addHandler(&asyncWebSocketMain);
                    asyncWebServer.addHandler(&asyncWebSocketSettings);
                    asyncWebServer.begin();
                    this->normalModeSubState = WebserverNormalModeSubState::HandleClients;
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
                asyncWebServer.end();
                asyncWebServer.reset();
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
void Webserver::ConfigurationWebpage(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", ConfigurationPage);
};

/**
 * @brief Reads the arguments from the configuration webpage and displays the configuration submitted webpage
 * 
 */
void Webserver::ConfigurationWebpageSubmitted(AsyncWebServerRequest *request)
{

    if (request->hasParam("wifiSSID", true))
    {
        this->configurationData.wifiSSID = request->getParam("wifiSSID", true)->value();
    }
    if (request->hasParam("wifiPassword", true))
    {
        this->configurationData.wifiPassword = request->getParam("wifiPassword", true)->value();
    }
    if (request->hasParam("mqttBrokerIpAddress", true))
    {
        this->configurationData.mqttBrokerIpAddress = request->getParam("mqttBrokerIpAddress", true)->value();
    }
    if (request->hasParam("mqttBrokerUsername", true))
    {
        this->configurationData.mqttBrokerUsername = request->getParam("mqttBrokerUsername", true)->value();
    }
    if (request->hasParam("mqttBrokerPassword", true))
    {
        this->configurationData.mqttBrokerPassword = request->getParam("mqttBrokerPassword", true)->value();
    }
    if (request->hasParam("mqttBrokerPort", true))
    {
        this->configurationData.mqttBrokerPort =
            strtol(request->getParam("mqttBrokerPort", true)->value().c_str(), NULL, 0);
    }
    if (request->hasParam("mqttClientName", true))
    {
        this->configurationData.mqttClientName = request->getParam("mqttClientName", true)->value();
    }
    this->configurationData.isConfigured = true;

    request->send_P(200, "text/html", SubmittedConfigurationPage);

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
void Webserver::ConfigurationNotFoundWebpage(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Configuration page not found");
}

/**
 * @brief Displays the main page in normal mode
 * 
 */
void Webserver::NormalMainWebpage(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", MainPage);
};

/**


/**
 * @brief Displays the settings page in normal mode
 * 
 */
void Webserver::NormalSettingsWebpage(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", SettingsPage);
};

/**
 * @brief Sends a 404 error message to the client with a "Normal page not found" message
 * 
 */
void Webserver::NormalNotFoundWebpage(AsyncWebServerRequest *request)
{
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
        if (this->filesystem->isLEDStateDataReady())
        {
            Serial.printf("ws[%u] Sending initial led state data \n", client->id());
            for (int i = 0; i < STRIP_COUNT; i++)
            {
                String msg = this->BuildWebsocketMessage("Power", String(i + 1), String(this->filesystem->getLEDStateData().Power[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("RedValue", String(i + 1), String(this->filesystem->getLEDStateData().RedValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("GreenValue", String(i + 1), String(this->filesystem->getLEDStateData().GreenValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("BlueValue", String(i + 1), String(this->filesystem->getLEDStateData().BlueValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("ColdWhiteValue", String(i + 1), String(this->filesystem->getLEDStateData().ColdWhiteValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("WarmWhiteValue", String(i + 1), String(this->filesystem->getLEDStateData().WarmWhiteValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("BrightnessValue", String(i + 1), String(this->filesystem->getLEDStateData().BrightnessValue[i]));
                server->text(client->id(), msg);
                msg = this->BuildWebsocketMessage("EffectValue", String(i + 1), String(this->helper->SingleLEDEffectToUint8(this->filesystem->getLEDStateData().EffectValue[i])));
                server->text(client->id(), msg);
            }
        }
        else
        {
            Serial.printf("ws[%u] Cant send initial led state data because filesystem is not ready \n", client->id());
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
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.Power[stripNumber - 1] = power;
                    String msg = this->BuildWebsocketMessage("Power", String(stripNumber), String(power));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ RedValue ================================ //
            else if (dataArray[0].equals("RedValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t redValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.RedValue[stripNumber - 1] = redValue;
                    String msg = this->BuildWebsocketMessage("RedValue", String(stripNumber), String(redValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ GreenValue ================================ //
            else if (dataArray[0].equals("GreenValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t greenValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.GreenValue[stripNumber - 1] = greenValue;
                    String msg = this->BuildWebsocketMessage("GreenValue", String(stripNumber), String(greenValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ BlueValue ================================ //
            else if (dataArray[0].equals("BlueValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t blueValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.BlueValue[stripNumber - 1] = blueValue;
                    String msg = this->BuildWebsocketMessage("BlueValue", String(stripNumber), String(blueValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ ColdWhiteValue ================================ //
            else if (dataArray[0].equals("ColdWhiteValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t coldWhiteValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.ColdWhiteValue[stripNumber - 1] = coldWhiteValue;
                    String msg = this->BuildWebsocketMessage("ColdWhiteValue", String(stripNumber), String(coldWhiteValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ WarmWhiteValue ================================ //
            else if (dataArray[0].equals("WarmWhiteValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t warmWhiteValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.WarmWhiteValue[stripNumber - 1] = warmWhiteValue;
                    String msg = this->BuildWebsocketMessage("WarmWhiteValue", String(stripNumber), String(warmWhiteValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ BrightnessValue ================================ //
            else if (dataArray[0].equals("BrightnessValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t brightnessValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.BrightnessValue[stripNumber - 1] = brightnessValue;
                    String msg = this->BuildWebsocketMessage("BrightnessValue", String(stripNumber), String(brightnessValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
                }
            }
            // ================================ EffectValue ================================ //
            else if (dataArray[0].equals("EffectValue"))
            {
                uint8_t stripNumber = dataArray[1].toInt();
                uint8_t effectValue = dataArray[2].toInt();

                if (stripNumber >= 1 && stripNumber <= STRIP_COUNT)
                {
                    LEDStateData ledStateData = this->filesystem->getLEDStateData();
                    ledStateData.EffectValue[stripNumber - 1] = this->helper->Uint8ToSingleLEDEffect(effectValue);
                    String msg = this->BuildWebsocketMessage("EffectValue", String(stripNumber), String(effectValue));
                    // We Broadcast the new data to all connected clients
                    server->textAll(msg);
                    this->filesystem->saveLEDState(ledStateData);
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
        if (this->filesystem->isSettingsDataReady())
        {
            Serial.printf("ws[%u] Sending initial settings data \n", client->id());
            for (int i = 0; i < STRIP_COUNT; i++)
            {
                for (int j = 0; j < CHANNEL_COUNT; j++)
                {
                    String msg = this->BuildWebsocketMessage("StripConfig", String(i + 1), String(j + 1), String(this->helper->LEDOutputTypeToUint8(this->filesystem->getSettingData().stripChannelOutputs[i][j])));
                    server->text(client->id(), msg);
                }
            }
        }
        else
        {
            Serial.printf("ws[%u] Cant send initial settings data because filesystem is not ready \n", client->id());
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
                        SettingsData settingsData = this->filesystem->getSettingData();
                        settingsData.stripChannelOutputs[stripNumber - 1][channelID - 1] = this->helper->Uint8ToLEDOutputType(outputType);
                        String msg = this->BuildWebsocketMessage("StripConfig", String(stripNumber), String(channelID), String(outputType));
                        // We Broadcast the new data to all connected clients
                        server->textAll(msg);
                        this->filesystem->saveSettings(settingsData);
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