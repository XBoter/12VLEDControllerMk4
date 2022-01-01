#pragma once

// Includes
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "../Network/Network.h"
#include "../Filesystem/Filesystem.h"

// Interface
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Network;
class Filesystem;

// Classes
class OTA : public IBaseClass
{
    // ## Constructor / Important ## //
public:
    OTA();
    void setReference(Network *network,
                      Filesystem *filesystem);
    bool init = false;

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    Network *network;
    Filesystem *filesystem;

public:
    // ## Functions ## //
private:
public:
};
