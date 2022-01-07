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

    // ======== Motion ======== //
    String motionDataFilename = "MotionData.dat";
    FilesystemMotionData motionData = {};
    bool motionDataReady = false;

    // ======== Configuration ======== //
    String configurationDataFilename = "ConfigurationData.dat";
    FilesystemConfigurationData configurationData = {};
    bool configurationDataReady = false;

    // ======== Settings ======== //
    String settingsStripDataFilename[STRIP_COUNT]{"SettingsStripData1.dat", "SettingsStripData2.dat"};
    FilesystemSettingsStripData settingsStripData[STRIP_COUNT]{};
    bool settingsStripDataReady[STRIP_COUNT]{false};

    // ================ LED State ================ //
    String ledStripDataFilename[STRIP_COUNT]{"LedStripData1.dat", "LedStripData2.dat"};
    FilesystemLEDStripData ledStripData[STRIP_COUNT]{};
    bool ledStripDataReady[STRIP_COUNT]{false};

    // ======== Other ======== //
    uint state = 0;
    const uint16_t FILE_LAST_WRITE_DELAY = 50;

    // ================ Methods ================ //
private:
    // ======== Motion ======== //
    FilesystemMotionData loadMotionData();
    // ======== Configuration ======== //
    FilesystemConfigurationData loadConfigurationData();
    // ======== Settings ======== //
    FilesystemSettingsStripData loadSettingsStripData(uint8_t stripID);
    // ======== LED State ======== //
    FilesystemLEDStripData loadLEDStripData(uint8_t stripID);
    // ======== File Operations ======== //
    void createFileIfMissing(String filename);
    void resetFileIfExists(String filename);
    void listAllFiles();

public:
    // ======== Motion Data ======== //
    void resetMotionData();
    void saveMotionData(FilesystemMotionData data);
    FilesystemMotionData getMotionData();
    bool isMotionDataReady();
    // ======== Configuration Data ======== //
    void resetConfigurationData();
    void saveConfigurationData(FilesystemConfigurationData data);
    FilesystemConfigurationData getConfigurationData();
    bool isConfigurationDataReady();
    // ======== Settings Data ======== //
    void resetSettingsStripData(uint8_t stripID);
    void saveSettingsStripData(uint8_t stripID, FilesystemSettingsStripData data);
    FilesystemSettingsStripData getSettingStripData(uint8_t stripID);
    bool isSettingsStripDataReady(uint8_t stripID);
    // ======== LED Strip Data ======== //
    void resetLEDStripData(uint8_t stripID);
    void saveLEDStripData(uint8_t stripID, FilesystemLEDStripData data);
    FilesystemLEDStripData getLEDStripData(uint8_t stripID);
    bool isLEDStripDataReady(uint8_t stripID);
};
