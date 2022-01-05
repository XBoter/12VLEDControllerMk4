#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include <LittleFS.h>
#include "../Helper/Helper.h"

#include "../Structs/Structs.h"
#include "../Parameterhandler/Parameterhandler.h"
#include "../Constants/Constants.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Helper;
class Parameterhandler;

// ================================ CLASS ================================ //
/**
 * @brief The Filesystem Class handles the management of the file saving/creating/loading/...
 */
class Filesystem : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Filesystem();
    void setReference(Helper *helper,
                      Parameterhandler *parameterhandler);
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
    Parameterhandler *parameterhandler;
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
    ConfigurationData loadConfiguration();
    // ======== Settings ======== //
    SettingsData loadSettings();
    // ======== LED State ======== //
    LEDStateData loadLEDState();
    // ======== File Operations ======== //
    void createFileIfMissing(String filename);
    void resetFileIfExists(String filename);
    void listAllFiles();

public:
    // ======== Configuration ======== //
    void resetConfiguration();
    void saveConfiguration(ConfigurationData data);
    ConfigurationData getConfigurationData();
    bool isConfigurationDataReady();
    // ======== Settings ======== //
    void resetSettings();
    void saveSettings(SettingsData data);
    SettingsData getSettingData();
    bool isSettingsDataReady();
    // ======== LED State ======== //
    void resetLEDState();
    void saveLEDState(LEDStateData data);
    LEDStateData getLEDStateData();
    bool isLEDStateDataReady();
};
