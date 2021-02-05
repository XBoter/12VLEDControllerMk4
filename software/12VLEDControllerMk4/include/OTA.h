#pragma once

// Includes
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Network.h"
#include "Configuration.h"

// Interface
#include "../interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Network;
class Configuration;

// Classes
class OTA : public IBaseClass
{
    // ## Constructor ## //
public:
    OTA();
    void setReference(Network *network,
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
