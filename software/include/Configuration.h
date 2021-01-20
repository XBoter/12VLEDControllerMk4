#pragma once

// Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Structs.h"
#include <FS.h>
#include <LittleFS.h>

// Interface
#include "../interface/IBaseClass.h"

// Classes
class Configuration : public IBaseClass
{
    // ## Constructor ## //
public:
    Configuration();

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    unsigned long prevMillisReset = 0;
    unsigned long prevMillisResetBlink = 0;

    unsigned long timeoutResetBlink = 300;
    unsigned long timeoutRest = 5000; // 5 sec

    bool ledOn = false;
    bool resetOrNotConfigured = false;
    uint state = 0;

    ConfiguredData data;
    uint configDataAddr = 0;

public:
    bool isFinished = false;

    // ## Functions ## //
private:
    void saveConfig();
    void loadConfig();
    void resetConfig();
    void createConfigIfMissing();
    void listConfigs();

public:
    void setData(ConfiguredData data);
    ConfiguredData getData();
    void formSubmitFinished();
};
