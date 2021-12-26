#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "../Structs/Structs.h"
#include "../src/Webpage/transformed_to_c/ConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/SubmittedConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/MainPage.h"
#include "../src/Webpage/transformed_to_c/SettingsPage.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

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
    void setReference();
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    // ======== onboard LED ======== //
    bool onboardLEDState = false;
    unsigned long prevMillisBlinkOnBoardLED = 0;

    // ======== Configuration ======== //
    uint configurationDataAdr = 0;
    bool ControllerConfigurationIsSet = false;

    IPAddress apIP = IPAddress(192, 168, 1, 1);

    unsigned long prevMillisReset = 0;
    unsigned long prevMillisResetBlink = 0;
    unsigned long prevMillisAPShutdown = 0;

    unsigned long timeoutResetBlink = 300;
    unsigned long timeoutRest = 5000;       // 5 sec
    unsigned long timeoutAPShutdown = 2000; // 2 sec

    uint state = 0;

    bool isInConfigurationMode = false;
    bool changeToConfigurationModeRequest = false;

    bool isInNormalMode = false;
    bool changeToNormalModeRequest = false;

    // ================ Functions ================ //
private:
    void inputFormConfiguration();
    void inputFormFilledConfiguration();
    void blinkOnBoardLED(uint16_t interval);
    void turnOffOnBoardLED();
    void ConfigurationModeHandler();
    void NormalModeHandler();

public:
    bool getConfigurationMode();
    void RequestChangeToConfigurationMode();
    bool getNormalMode();
    void RequestChangeToNormalMode();
};
