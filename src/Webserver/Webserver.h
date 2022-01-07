#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "../Structs/Structs.h"
#include "../Filesystem/Filesystem.h"
#include "../Helper/Helper.h"
#include "../Network/Network.h"
#include "../Parameterhandler/Parameterhandler.h"
#include "../Constants/Constants.h"
#include "../src/Webpage/transformed_to_c/ConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/SubmittedConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/MainPage.h"
#include "../src/Webpage/transformed_to_c/SettingsPage.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Filesystem;
class Helper;
class Network;
class Parameterhandler;

// ================================ CLASS ================================ //
/**
 * @brief The Webserver Class handles the configuration of the LED Controller.
 * Handles the "offline control" when we are not connected to an mqtt broker or WiFi via a webpage
 * handles the settings for the led strip output configuration via a webpage
 * 
 */
class Webserver : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Webserver();
    void setReference(Filesystem *filesystem,
                      Helper *helper,
                      Network *network,
                      Parameterhandler *parameterhandler);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    Filesystem *filesystem;
    Helper *helper;
    Network *network;
    Parameterhandler *parameterhandler;

    // ======== Webserver / Websocket ======== //
    AsyncWebServer asyncWebServer = AsyncWebServer(80);
    AsyncWebSocket asyncWebSocketMain = AsyncWebSocket("/ws/main");
    AsyncWebSocket asyncWebSocketSettings = AsyncWebSocket("/ws/settings");
    unsigned long prevMillisWebsocketCleanup = 0;
    const uint16_t timeoutWebsocketCleanup = 500; // 500 ms
    AsyncWebHandler *indexHandle;
    AsyncWebHandler *submittedHandle;
    AsyncWebHandler *settingsHandle;
    uint8_t webserverResetState = 0;

    // ======== Configuration Mode ======== //
    unsigned long prevMillisConfigurationMode = 0;
    unsigned long timeoutConfigurationMode = 5000; // 5 sec
    WebserverConfigurationModeState configurationModeState = WebserverConfigurationModeState::StartConfigurationMode;
    WebserverConfigurationModeSubState configurationModeSubState = WebserverConfigurationModeSubState::BeginWebserver;
    bool isInConfigurationMode = false;
    bool shutdownConfigurationMode = true;
    bool changeToConfigurationModeRequest = false;

    // ======== Normal Mode ======== //
    unsigned long prevMillisNormalMode = 0;
    unsigned long timeoutNormalMode = 5000; // 5 sec
    WebserverNormalModeState normalModeState = WebserverNormalModeState::StartNormalMode;
    WebserverNormalModeSubState normalModeSubState = WebserverNormalModeSubState::BeginWebserver;
    bool isInNormalMode = true;
    bool shutdownNormalMode = false;
    bool changeToNormalModeRequest = false;

    // ======== mDNS ======== //
    bool restartMDNS = false;
    uint8_t restartStateMDNS = 20;
    bool mDNSInit = false;

    // ================ Functions ================ //
private:
    // ================ Webpages ================ //
    // ======== Configuration mode
    // ==== Handler
    void ConfigurationModeHandler(bool shutdown);
    // ==== Webpages
    void ConfigurationWebpage(AsyncWebServerRequest *request);
    void ConfigurationWebpageSubmitted(AsyncWebServerRequest *request);
    void ConfigurationNotFoundWebpage(AsyncWebServerRequest *request);
    // ======== Normal Mode
    // ==== Handler
    void NormalModeHandler(bool shutdown);
    // ==== Webpages
    void NormalMainWebpage(AsyncWebServerRequest *request);
    void NormalSettingsWebpage(AsyncWebServerRequest *request);
    void NormalNotFoundWebpage(AsyncWebServerRequest *request);

    // ================ Websocket ================ //
    void WebSocketEventMain(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void WebSocketEventSettings(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    String BuildWebsocketMessage(String type, String data1, String data2, String data3);
    String BuildWebsocketMessage(String type, String data1, String data2);
    String BuildWebsocketMessage(String type, String data1);

    // ================ MDNS ================ //
    void HandleMDNS();

public:
    bool getConfigurationMode();
    void RequestChangeToConfigurationMode();
    bool getNormalMode();
    void RequestChangeToNormalMode();
};
