#pragma once

#define GHOTA_USER "XBoter"
#define GHOTA_REPO "12VLEDControllerMk4"
#define GHOTA_CURRENT_TAG "1.2.0"
#define GHOTA_BIN_FILE "12VLEDControllerMk4.ino.nodemcu.bin"
#define GHOTA_ACCEPT_PRERELEASE 0

// Includes
#include <Arduino.h>
#include <LittleFS.h>
#include "Network.h"
#include "Structs.h"
#include <CertStoreBearSSL.h>
#include <ESP_OTA_GitHub.h>
#include <LittleFS.h>

// Interface
#include "../interface/IBaseClass.h"

// Classes
class OTAGit : public IBaseClass
{
    // ## Constructor ## //
public:
    OTAGit(Network *network);

    // ## Interface ## //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ## Data ## //
private:
    BearSSL::CertStore certStore;
    Network *network;

public:
    // ## Functions ## //
private:
    void HandleUpgrade();

public:
};
