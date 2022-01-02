#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

#include "../Structs/Structs.h"
#include "../Filesystem/Filesystem.h"
#include "../Helper/Helper.h"
#include "../Network/Network.h"
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
                      Network *network);
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

    // ======== OnBoard LED ======== //
    bool onboardLEDState = false;
    unsigned long prevMillisBlinkOnBoardLED = 0;

    // ======== Configuration Mode ======== //
    unsigned long prevMillisConfigurationMode = 0;
    unsigned long timeoutConfigurationMode = 5000; // 5 sec
    ConfigurationData configurationData = {};
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
    bool mDNSInit = false;

    // ================ Functions ================ //
private:
    // ======== OnBoard LED ======== //
    void blinkOnBoardLED(uint16_t interval);
    void turnOffOnBoardLED();
    // ======== Configuration Mode ======== //
    // ==== Handler
    void ConfigurationModeHandler(bool shutdown);
    // ==== Webpages
    void ConfigurationWebpage();
    void ConfigurationWebpageSubmitted();
    void ConfigurationNotFoundWebpage();
    // ======== Normal Mode ======== //
    // ==== Handler
    void NormalModeHandler(bool shutdown);
    // ==== Webpages
    void NormalMainWebpage();
    void NormalMainWebpageEvent();
    void NormalSettingsWebpage();
    void NormalSettingsWebpageEvent();
    void NormalNotFoundWebpage();

    // ================ Websocket ================ //
    void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

public:
    bool getConfigurationMode();
    void RequestChangeToConfigurationMode();
    bool getNormalMode();
    void RequestChangeToNormalMode();
};
