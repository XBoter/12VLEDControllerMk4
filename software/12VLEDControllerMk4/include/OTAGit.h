#pragma once

// Defines
#define GHOTA_USER "XBoter"
#define GHOTA_REPO "12VLEDControllerMk4"
#define GHOTA_CURRENT_TAG "2.0.0"
#define GHOTA_BIN_FILE "github_esp_ota_test.ino.esp8266.bin"
#define GHOTA_ACCEPT_PRERELEASE 0

// Includes
#include <Arduino.h>
#include <LittleFS.h>
#include "Network.h"
#include "Structs.h"
#include <ESP_OTA_GitHub.h>
#include <CertStoreBearSSL.h>

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
    Network *network;
    BearSSL::CertStore certStore;

    bool foundCertificates = false;

public:
    // ## Functions ## //
private:
    void HandleUpgrade();

public:
};
