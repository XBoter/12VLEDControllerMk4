#pragma once

// ================================ INCLUDES ================================ //
#include <Arduino.h>
#include "../Filesystem/Filesystem.h"
#include "../Network/Network.h"
#include "../Structs/Structs.h"
#include "../Constants/Constants.h"

// ================================ INTERFACES ================================ //
#include "../Interface/IBaseClass.h"

// Blueprint for compiler. Problem => circular dependency
class Filesystem;
class Network;

// ================================ CLASS ================================ //
/**
 * @brief Handles the management off all the diffrent parameters.
 * 
 */
class Parameterhandler : public IBaseClass
{
    // ================ Constructor / Reference ================ //
public:
    Parameterhandler();
    void setReference(Filesystem *filesystem,
                      Network *network);
    bool init = false;

    // ================ Interface ================ //
private:
public:
    virtual bool Init();
    virtual void Run();

    // ================ Data ================ //
private:
    Filesystem *filesystem;
    Network *network;

    // ==== Motion
    MotionParameter motionParameter = {};
    // ==== LED Strip
    LEDStripParameter ledStripParameter[STRIP_COUNT]{};
    // ==== Settings
    SettingsStripParameter settingsStripParameter[STRIP_COUNT]{};
    // ==== Configuration
    ConfigurationParameter configurationParameter = {};

public:
    // ================ Methods ================ //
private:
public:
    // ==== Motion
    MotionParameter getMotionParameter();
    void updateMotionParameter(MotionParameter data);
    void updateMotionParameter(FilesystemMotionData data);
    void updateMotionParameter(NetworkMotionData data);
    // ==== LED Strip
    LEDStripParameter getLEDStripParameter(uint8_t stripID);
    void updateLEDStripParameter(uint8_t stripID, LEDStripParameter data);
    void updateLEDStripParameter(uint8_t stripID, FilesystemLEDStripData data);
    void updateLEDStripParameter(uint8_t stripID, NetworkLEDStripData data);
    // ==== Settings
    SettingsStripParameter getSettingsStripParameter(uint8_t stripID);
    void updateSettingsStripParameter(uint8_t stripID, SettingsStripParameter data);
    void updateSettingsStripParameter(uint8_t stripID, FilesystemSettingsStripData data);
    // ==== Configuration
    ConfigurationParameter getConfigurationParameter();
    void updateConfigurationParameter(ConfigurationParameter data);
    void updateConfigurationParameter(FilesystemConfigurationData data);
};
