#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <LittleFS.h>
#include "../Helper/Helper.h"

#include "../Structs/Structs.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"


// Blueprint for compiler. Problem => circular dependency
class Helper;

// ================================ CLASS ================================ //
/**
 * @brief The Filesystem Class handles the management of the file saving/creating/loading/...
 */
class Filesystem : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Filesystem();
    void setReference(Helper *helper);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    // ======== External Components ======== //
    Helper *helper;
    // ======== Configuration ======== //
    String configurationFilename = "config.txt";
    ConfigurationData configurationData = {};
    bool configurationDataReady = false;

    // ======== Settings ======== //
    String settingsFilename = "settings.txt";
    SettingsData settingsData = {};
    bool settingsDataReady = false;

    // ================ LED State ================ //
    String ledStateFilename = "ledstate.txt";
    LEDStateData ledStateData = {};
    bool ledStateDataReady = false;

    // ======== Other ======== //
    uint state = 0;

    // ================ Methods ================ //
private:
    // ======== Configuration ======== //
    void saveConfiguration(ConfigurationData data);
    ConfigurationData loadConfiguration();
    // ======== Settings ======== //
    void saveSettings(SettingsData data);
    SettingsData loadSettings();
    // ======== LED State ======== //
    void saveLEDState(LEDStateData data);
    LEDStateData loadLEDState();
    // ======== File Operations ======== //
    void createFileIfMissing(String filename);
    void resetFileIfExists(String filename);
    void listAllFiles();

public:
    // ======== Config ======== //
    ConfigurationData getConfigurationData();
    bool isConfigurationDataReady();
    // ======== Settings ======== //
    SettingsData getSettingData();
    bool isSettingsDataReady();
    // ======== LED State ======== //
    LEDStateData getLEDStateData();
    bool isLEDStateDataReady();
};
