#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClient.h>

#include "../LedDriver/LedDriver.h"
#include "../Structs/Structs.h"
#include "../src/Webpage/transformed_to_c/ConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/SubmittedConfigurationPage.h"
#include "../src/Webpage/transformed_to_c/MainPage.h"
#include "../src/Webpage/transformed_to_c/SettingsPage.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class LedDriver;

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
    void setReference(LedDriver *ledDriver);
    LedDriver *ledDriver;
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    // ======== Configuration ======== //
    ConfigurationData configurationData = {};
    uint configurationDataAdr = 0;

    IPAddress apIP = IPAddress(192, 168, 1, 1);

    unsigned long prevMillisReset = 0;
    unsigned long prevMillisResetBlink = 0;
    unsigned long prevMillisAPShutdown = 0;

    unsigned long timeoutResetBlink = 300;
    unsigned long timeoutRest = 5000;       // 5 sec
    unsigned long timeoutAPShutdown = 2000; // 2 sec

    bool ledOn = false;
    bool resetOrNotConfigured = false;
    uint state = 0;

    // ======== Settings ======== //
    SettingsData settingsData = {};
    uint settingsDataAdr = 0;

    // ================ LED State ================ //
    LEDStateData ledStateData = {};
    uint ledStateDataAdr = 0;

    // ======== On Board LED ======== //

private:
    // ======== Configuration ======== //
    void saveConfiguration();
    void loadConfiguration();
    void resetConfiguration();
    void createConfiguration();
    void inputFormConfiguration();
    void inputFormFilledConfiguration();
    // ======== Settings ======== //
    void saveSettings();
    void loadSettings();
    void resetSettings();
    void createSettings();
    // ======== LED State ======== //
    void saveLEDState();
    void loadLEDState();
    void resetLEDState();
    void createLEDState();
    // ======== File Operations ======== //
    void listFiles();
    // ======== On Board LED ======== //
    void blinkOnBoardLED(uint16_t interval);
    void turnOffOnBoardLED();

public:
    // ======== Config ======== //
    ConfigurationData getConfiguredData();
    bool isConfigurationDataReady();
    // ======== Settings ======== //
    SettingsData getSettingData();
    bool isSettingsDataReady();
    // ======== LED State ======== //
    LEDStateData getLEDStateData();
    bool isLEDStateDataReady();
};
