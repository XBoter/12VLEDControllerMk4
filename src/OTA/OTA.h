#pragma once

// Includes
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "../Network/Network.h"
#include "../Configuration/Configuration.h"

// Interface
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Network;
class Configuration;

// Classes
class OTA : public IBaseClass
{
    // ## Constructor / Important ## //
public:
    OTA();
    void setReference(Network *network,
                      Configuration *configuration);
    bool init = false;

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
