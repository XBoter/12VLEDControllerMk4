#pragma once

// Includes
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "../Network/Network.h"
#include "../Webserver/Webserver.h"

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
                      Webserver *webserver);
    bool init = false;

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    Network *network;
    Webserver *webserver;

public:
    // ## Functions ## //
private:
public:
};
