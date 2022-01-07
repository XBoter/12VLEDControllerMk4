#include "Parameterhandler.h"

/**
 * @brief Constructor for the parameterhandler class
 */
Parameterhandler::Parameterhandler(){

};

/**
 * @brief Sets the needed refernce for the parameterhandler class
 */
void Parameterhandler::setReference(Filesystem *filesystem,
                                    Network *network)
{
    this->filesystem = filesystem;
    this->network = network;
};

/**
 * @brief Initializes the parameterhandler component
 * 
 * @return True if the initialization was successful
 */
bool Parameterhandler::Init()
{
    if (!this->init)
    {
        this->init = true;
    }

    return this->init;
};

/**
 * @brief Runs the parameterhandler component
 * 
 */
void Parameterhandler::Run()
{
    if (!init)
    {
        Init();
        return;
    }
};

// ================================================================ Motion ================================================================ //
MotionParameter Parameterhandler::getMotionParameter()
{
    return this->motionParameter;
}

void Parameterhandler::updateMotionParameter(MotionParameter data)
{
    this->motionParameter = data;

    FilesystemMotionData filesystemMotionData = this->filesystem->getMotionData();

    // We cant use static cast anymore because we only got the parant struct of the child struct and therefore lost some information
    filesystemMotionData.MotionDetectionEnabled = data.MotionDetectionEnabled;
    filesystemMotionData.TimeBasedBrightnessChangeEnabled = data.TimeBasedBrightnessChangeEnabled;
    filesystemMotionData.MotionDetectionTimeout = data.MotionDetectionTimeout;
    filesystemMotionData.Red = data.Red;
    filesystemMotionData.Green = data.Green;
    filesystemMotionData.Blue = data.Blue;
    filesystemMotionData.ColorFadeTime = data.ColorFadeTime;
    filesystemMotionData.ColorFadeCurve = data.ColorFadeCurve;
    filesystemMotionData.ColorBrightness = data.ColorBrightness;
    filesystemMotionData.ColorBrightnessFadeTime = data.ColorBrightnessFadeTime;
    filesystemMotionData.ColorBrightnessFadeCurve = data.ColorBrightnessFadeCurve;
    filesystemMotionData.WhiteTemperature = data.WhiteTemperature;
    filesystemMotionData.WhiteTemperatureFadeTime = data.WhiteTemperatureFadeTime;
    filesystemMotionData.WhiteTemperatureFadeCurve = data.WhiteTemperatureFadeCurve;
    filesystemMotionData.WhiteTemperatureBrightness = data.WhiteTemperatureBrightness;
    filesystemMotionData.WhiteTemperatureBrightnessFadeTime = data.WhiteTemperatureBrightnessFadeTime;
    filesystemMotionData.WhiteTemperatureBrightnessFadeCurve = data.WhiteTemperatureBrightnessFadeCurve;

    this->filesystem->saveMotionData(filesystemMotionData);

    NetworkMotionData networkMotionData = this->network->getNetworkMotionData();

    networkMotionData.MotionDetectionEnabled = data.MotionDetectionEnabled;
    networkMotionData.TimeBasedBrightnessChangeEnabled = data.TimeBasedBrightnessChangeEnabled;
    networkMotionData.MotionDetectionTimeout = data.MotionDetectionTimeout;
    networkMotionData.Red = data.Red;
    networkMotionData.Green = data.Green;
    networkMotionData.Blue = data.Blue;
    networkMotionData.ColorFadeTime = data.ColorFadeTime;
    networkMotionData.ColorFadeCurve = data.ColorFadeCurve;
    networkMotionData.ColorBrightness = data.ColorBrightness;
    networkMotionData.ColorBrightnessFadeTime = data.ColorBrightnessFadeTime;
    networkMotionData.ColorBrightnessFadeCurve = data.ColorBrightnessFadeCurve;
    networkMotionData.WhiteTemperature = data.WhiteTemperature;
    networkMotionData.WhiteTemperatureFadeTime = data.WhiteTemperatureFadeTime;
    networkMotionData.WhiteTemperatureFadeCurve = data.WhiteTemperatureFadeCurve;
    networkMotionData.WhiteTemperatureBrightness = data.WhiteTemperatureBrightness;
    networkMotionData.WhiteTemperatureBrightnessFadeTime = data.WhiteTemperatureBrightnessFadeTime;
    networkMotionData.WhiteTemperatureBrightnessFadeCurve = data.WhiteTemperatureBrightnessFadeCurve;

    this->network->UpdateNetworkMotionData(networkMotionData);
}

void Parameterhandler::updateMotionParameter(FilesystemMotionData data)
{
    this->updateMotionParameter(static_cast<MotionParameter>(data));
}

void Parameterhandler::updateMotionParameter(NetworkMotionData data)
{
    this->updateMotionParameter(static_cast<MotionParameter>(data));
}

// ================================================================ LED Strip ================================================================ //
LEDStripParameter Parameterhandler::getLEDStripParameter(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        return this->ledStripParameter[stripID];
    }
}

void Parameterhandler::updateLEDStripParameter(uint8_t stripID, LEDStripParameter data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->ledStripParameter[stripID] = data;

        FilesystemLEDStripData filesystemLEDStripData = this->filesystem->getLEDStripData(stripID);

        // We cant use static cast anymore because we only got the parant struct of the child struct and therefore lost some information
        filesystemLEDStripData.Power = data.Power;
        filesystemLEDStripData.Red = data.Red;
        filesystemLEDStripData.Green = data.Green;
        filesystemLEDStripData.Blue = data.Blue;
        filesystemLEDStripData.ColorFadeTime = data.ColorFadeTime;
        filesystemLEDStripData.ColorFadeCurve = data.ColorFadeCurve;
        filesystemLEDStripData.ColorBrightness = data.ColorBrightness;
        filesystemLEDStripData.ColorBrightnessFadeTime = data.ColorBrightnessFadeTime;
        filesystemLEDStripData.ColorBrightnessFadeCurve = data.ColorBrightnessFadeCurve;
        filesystemLEDStripData.WhiteTemperature = data.WhiteTemperature;
        filesystemLEDStripData.WhiteTemperatureFadeTime = data.WhiteTemperatureFadeTime;
        filesystemLEDStripData.WhiteTemperatureFadeCurve = data.WhiteTemperatureFadeCurve;
        filesystemLEDStripData.WhiteTemperatureBrightness = data.WhiteTemperatureBrightness;
        filesystemLEDStripData.WhiteTemperatureBrightnessFadeTime = data.WhiteTemperatureBrightnessFadeTime;
        filesystemLEDStripData.WhiteTemperatureBrightnessFadeCurve = data.WhiteTemperatureBrightnessFadeCurve;
        filesystemLEDStripData.Effect = data.Effect;

        this->filesystem->saveLEDStripData(stripID, filesystemLEDStripData);

        NetworkLEDStripData networkLEDStripData = this->network->getNetworkLEDStripData(stripID);

        networkLEDStripData.Power = data.Power;
        networkLEDStripData.Red = data.Red;
        networkLEDStripData.Green = data.Green;
        networkLEDStripData.Blue = data.Blue;
        networkLEDStripData.ColorFadeTime = data.ColorFadeTime;
        networkLEDStripData.ColorFadeCurve = data.ColorFadeCurve;
        networkLEDStripData.ColorBrightness = data.ColorBrightness;
        networkLEDStripData.ColorBrightnessFadeTime = data.ColorBrightnessFadeTime;
        networkLEDStripData.ColorBrightnessFadeCurve = data.ColorBrightnessFadeCurve;
        networkLEDStripData.WhiteTemperature = data.WhiteTemperature;
        networkLEDStripData.WhiteTemperatureFadeTime = data.WhiteTemperatureFadeTime;
        networkLEDStripData.WhiteTemperatureFadeCurve = data.WhiteTemperatureFadeCurve;
        networkLEDStripData.WhiteTemperatureBrightness = data.WhiteTemperatureBrightness;
        networkLEDStripData.WhiteTemperatureBrightnessFadeTime = data.WhiteTemperatureBrightnessFadeTime;
        networkLEDStripData.WhiteTemperatureBrightnessFadeCurve = data.WhiteTemperatureBrightnessFadeCurve;
        networkLEDStripData.Effect = data.Effect;

        this->network->UpdateNetworkLEDStripData(stripID, networkLEDStripData);
    }
}

void Parameterhandler::updateLEDStripParameter(uint8_t stripID, FilesystemLEDStripData data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->updateLEDStripParameter(stripID, static_cast<LEDStripParameter>(data));
    }
}

void Parameterhandler::updateLEDStripParameter(uint8_t stripID, NetworkLEDStripData data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->updateLEDStripParameter(stripID, static_cast<LEDStripParameter>(data));
    }
}

// ================================================================ Settings ================================================================ //
SettingsStripParameter Parameterhandler::getSettingsStripParameter(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        return this->settingsStripParameter[stripID];
    }
}

void Parameterhandler::updateSettingsStripParameter(uint8_t stripID, SettingsStripParameter data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->settingsStripParameter[stripID] = data;

        FilesystemSettingsStripData filesystemSettingsStripData = this->filesystem->getSettingStripData(stripID);

        // We cant use static cast anymore because we only got the parant struct of the child struct and therefore lost some information
        for (int i = 0; i < CHANNEL_COUNT; i++)
        {
            filesystemSettingsStripData.ChannelOutputType[i] = data.ChannelOutputType[i];
        }

        this->filesystem->saveSettingsStripData(stripID, filesystemSettingsStripData);
    }
}

void Parameterhandler::updateSettingsStripParameter(uint8_t stripID, FilesystemSettingsStripData data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->updateSettingsStripParameter(stripID, static_cast<SettingsStripParameter>(data));
    }
}

// ================================================================ Configuration ================================================================ //
ConfigurationParameter Parameterhandler::getConfigurationParameter()
{
    return this->configurationParameter;
}

void Parameterhandler::updateConfigurationParameter(ConfigurationParameter data)
{
    this->configurationParameter = data;

    FilesystemConfigurationData filesystemConfigurationData = this->filesystem->getConfigurationData();

    // We cant use static cast anymore because we only got the parant struct of the child struct and therefore lost some information
    filesystemConfigurationData.WiFiSSID = data.WiFiSSID;
    filesystemConfigurationData.WiFiPassword = data.WiFiPassword;
    filesystemConfigurationData.MQTTBrokerIpAddress = data.MQTTBrokerIpAddress;
    filesystemConfigurationData.MQTTBrokerPort = data.MQTTBrokerPort;
    filesystemConfigurationData.MQTTBrokerUsername = data.MQTTBrokerUsername;
    filesystemConfigurationData.MQTTBrokerPassword = data.MQTTBrokerPassword;
    filesystemConfigurationData.MQTTClientName = data.MQTTClientName;

    this->filesystem->saveConfigurationData(filesystemConfigurationData);
}

void Parameterhandler::updateConfigurationParameter(FilesystemConfigurationData data)
{
    this->updateConfigurationParameter(static_cast<ConfigurationParameter>(data));
}