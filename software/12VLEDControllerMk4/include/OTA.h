#pragma once

// Includes
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Network.h"

// Interface
#include "../interface/IBaseClass.h"

// Classes
class OTA : public IBaseClass
{
    // ## Constructor ## //
public:
    OTA(Network *network,
        Configuration *configuration);

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    Network *network;
    Configuration *configuration;

public:
    // ## Functions ## //
private:
public:
};
