#pragma once

// Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Structs.h"
#include "LedDriver.h"
#include <LittleFS.h>

// Interface
#include "../interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class LedDriver;

// Classes
class Configuration : public IBaseClass
{
    // ## Constructor / Important ## //
public:
    Configuration();
    void setReference(LedDriver *ledDriver);
    bool init = false;

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    LedDriver *ledDriver;

    unsigned long prevMillisReset = 0;
    unsigned long prevMillisResetBlink = 0;
    unsigned long prevMillisAPShutdown = 0;

    unsigned long timeoutResetBlink = 300;
    unsigned long timeoutRest = 5000;       // 5 sec
    unsigned long timeoutAPShutdown = 2000; // 2 sec

    bool ledOn = false;
    bool resetOrNotConfigured = false;
    uint state = 0;

    uint configDataAddr = 0;

public:
    ConfiguredData data = {};
    bool isFinished = false;

    // ## Functions ## //
private:
    void saveConfig();
    void loadConfig();
    void resetConfig();
    void createConfig();
    void listFiles();

    void inputForm();
    void inputFormFilled();

public:
    ConfiguredData getData();
};
