#include "Filesystem.h"

/**
 * @brief Construct a new Filesystem:: Filesystem object
 * 
 */
Filesystem::Filesystem(){

};

/**
 * @brief Sets the needed refernce for the filesystem
 */
void Filesystem::setReference(Helper *helper,
                              Parameterhandler *parameterhandler)
{
    this->helper = helper;
    this->parameterhandler = parameterhandler;
};

/**
 * @brief Initializes the filesystem component
 * 
 * @return True if the initialization was successful
 */
bool Filesystem::Init()
{
    if (!init)
    {

        // == Start LittleFS
        Serial.println("Mount LittleFS");
        if (!LittleFS.begin())
        {
            Serial.println("LittleFS mount Failed!");
            return init;
        }
        else
        {
            Serial.println("LittleFS mount OK!");
            Serial.println("");

            FSInfo fsInfo;
            LittleFS.info(fsInfo);

            this->helper->TopSpacerPrint();
            Serial.println("LittleFS Info");

            Serial.print("Total space       : ");
            Serial.print(fsInfo.totalBytes);
            Serial.println("byte");

            Serial.print("Total space used  : ");
            Serial.print(fsInfo.usedBytes);
            Serial.println("byte");

            Serial.print("Block size        : ");
            Serial.print(fsInfo.blockSize);
            Serial.println("byte");

            Serial.print("Page size         : ");
            Serial.print(fsInfo.totalBytes);
            Serial.println("byte");

            Serial.print("Max open files    : ");
            Serial.println(fsInfo.maxOpenFiles);

            Serial.print("Max path length   : ");
            Serial.println(fsInfo.maxPathLength);

            this->helper->BottomSpacerPrint();
        }

        // == Create files if missing
        this->createFileIfMissing(this->configurationDataFilename);
        this->createFileIfMissing(this->motionDataFilename);
        for (int i = 0; i < STRIP_COUNT; i++)
        {
            this->createFileIfMissing(this->settingsStripDataFilename[i]);
            this->createFileIfMissing(this->ledStripDataFilename[i]);
        }

        // == List all found files in LittleFS
        this->listAllFiles();

        // == Load all files
        this->loadMotionData();
        this->loadConfigurationData();
        for (int i = 0; i < STRIP_COUNT; i++)
        {
            this->loadSettingsStripData(i);
            this->loadLEDStripData(i);
        }

        Serial.println(F("Filesystem initialized"));
        init = true;
    }
    return init;
};

/**
 * Runs the filesystem component
 */
void Filesystem::Run()
{
    if (!init)
    {
        return;
    }

    // == Create Inital data on new installation (Not Configuration because it needs to be set via the web interface)
    if (this->isMotionDataReady())
    {
        if (!this->getMotionData().isConfigured)
        {

            this->motionData.MotionDetectionEnabled = false;
            this->motionData.TimeBasedBrightnessChangeEnabled = false;
            this->motionData.MotionDetectionTimeout = 5000;

            this->motionData.Red = 255;
            this->motionData.Green = 127;
            this->motionData.Blue = 0;
            this->motionData.ColorFadeTime = 1000;
            this->motionData.ColorFadeCurve = FadeCurve::EaseInOut;
            this->motionData.ColorBrightness = 127;
            this->motionData.ColorBrightnessFadeTime = 1000;
            this->motionData.ColorBrightnessFadeCurve = FadeCurve::EaseInOut;

            this->motionData.WhiteTemperature = 250;
            this->motionData.WhiteTemperatureFadeTime = 1000;
            this->motionData.WhiteTemperatureFadeCurve = FadeCurve::EaseInOut;
            this->motionData.WhiteTemperatureBrightness = 0;
            this->motionData.WhiteTemperatureBrightnessFadeTime = 1000;
            this->motionData.WhiteTemperatureBrightnessFadeCurve = FadeCurve::EaseInOut;

            this->motionData.isConfigured = true;
            this->saveMotionData(this->motionData);
        }
    }
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (this->isSettingsStripDataReady(i))
        {
            if (!this->getSettingStripData(i).isConfigured)
            {
                this->settingsStripData[i].ChannelOutputType[0] = LEDOutputType::CW;
                this->settingsStripData[i].ChannelOutputType[1] = LEDOutputType::B;
                this->settingsStripData[i].ChannelOutputType[2] = LEDOutputType::R;
                this->settingsStripData[i].ChannelOutputType[3] = LEDOutputType::G;
                this->settingsStripData[i].ChannelOutputType[4] = LEDOutputType::WW;

                this->settingsStripData[i].ChannelOutputType[0] = LEDOutputType::CW;
                this->settingsStripData[i].ChannelOutputType[1] = LEDOutputType::B;
                this->settingsStripData[i].ChannelOutputType[2] = LEDOutputType::R;
                this->settingsStripData[i].ChannelOutputType[3] = LEDOutputType::G;
                this->settingsStripData[i].ChannelOutputType[4] = LEDOutputType::WW;

                this->settingsStripData[i].isConfigured = true;
                this->saveSettingsStripData(i, this->settingsStripData[i]);
            }
        }

        if (this->isLEDStripDataReady(i))
        {
            if (!this->getLEDStripData(i).isConfigured)
            {
                this->ledStripData[i].Power = false;

                this->ledStripData[i].Red = 0;
                this->ledStripData[i].Green = 0;
                this->ledStripData[i].Blue = 0;
                this->ledStripData[i].ColorFadeTime = 1000;
                this->ledStripData[i].ColorFadeCurve = FadeCurve::EaseInOut;
                this->ledStripData[i].ColorBrightness = 0;
                this->ledStripData[i].ColorBrightnessFadeTime = 1000;
                this->ledStripData[i].ColorBrightnessFadeCurve = FadeCurve::EaseInOut;

                this->ledStripData[i].WhiteTemperature = 250;
                this->ledStripData[i].WhiteTemperatureFadeTime = 1000;
                this->ledStripData[i].WhiteTemperatureFadeCurve = FadeCurve::EaseInOut;
                this->ledStripData[i].WhiteTemperatureBrightness = 0;
                this->ledStripData[i].WhiteTemperatureBrightnessFadeTime = 1000;
                this->ledStripData[i].WhiteTemperatureBrightnessFadeCurve = FadeCurve::EaseInOut;

                this->ledStripData[i].Effect = SingleLEDEffect::None;

                this->ledStripData[i].isConfigured = true;
                this->saveLEDStripData(i, this->ledStripData[i]);
            }
        }
    }
};

/**
 * @brief Returns the loaded FilesystemMotionData if 'motionDataReady' is true
 * 
 * @return The loaded FilesystemMotionData from the filesystem
 */
FilesystemMotionData Filesystem::getMotionData()
{
    if (this->motionDataReady)
    {
        return this->motionData;
    }
};

/**
 * @brief Returns the loaded FilesystemConfigurationData if 'configurationDataReady' is true
 * 
 * @return The loaded FilesystemConfigurationData from the filesystem
 */
FilesystemConfigurationData Filesystem::getConfigurationData()
{
    if (this->configurationDataReady)
    {
        return this->configurationData;
    }
};

/**
 * @brief Returns the loaded FilesystemSettingsData if 'settingsStripDataReady' is true
 * 
 * @return The loaded FilesystemSettingsData from the filesystem
 */
FilesystemSettingsStripData Filesystem::getSettingStripData(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        if (this->settingsStripDataReady[stripID])
        {
            return this->settingsStripData[stripID];
        }
    }
};

/**
 * @brief Returns the loaded FilesystemLEDStripData if 'ledStripDataReady' is true
 * 
 * @return The loaded FilesystemLEDStripData from the filesystem
 */
FilesystemLEDStripData Filesystem::getLEDStripData(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        if (this->ledStripDataReady[stripID])
        {
            return this->ledStripData[stripID];
        }
    }
};

/**
 * @brief Saves the motion data to the file on the filesystem
 * 
 * @param data The motion data to save
 */
void Filesystem::saveMotionData(FilesystemMotionData data)
{
    Serial.println(F("Saving motion data"));
    Serial.println(F(""));

    File file = LittleFS.open("/" + this->motionDataFilename, "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }
    else
    {
        file.write((byte *)&data, sizeof(data));
    }

    delay(this->FILE_LAST_WRITE_DELAY);
    file.close();
    this->motionData = data;
    this->motionDataReady = true;
};

/**
 * @brief Saves the configuration data to the file on the filesystem
 * 
 * @param data The configuration data to save
 */
void Filesystem::saveConfigurationData(FilesystemConfigurationData data)
{
    Serial.println(F("Saving configuration data"));

    /*
        We save and the load the data via the File.println because we are using dynamic types like string and that wont work with the byte read/write
    */

    File file = LittleFS.open("/" + this->configurationDataFilename, "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }
    else
    {
        // ==== WiFiSSID ==== //
        if (!file.println(data.WiFiSSID))
        {
            Serial.println(F("WiFiSSID failed to save"));
        }

        // ==== WiFiPassword ==== //
        if (!file.println(data.WiFiPassword))
        {
            Serial.println(F("WiFiPassword failed to save"));
        }

        // ==== MQTTBrokerIpAddress ==== //
        if (!file.println(data.MQTTBrokerIpAddress))
        {
            Serial.println(F("MQTTBrokerIpAddress failed to save"));
        }

        // ==== MQTTBrokerPort ==== //
        if (!file.println(String(data.MQTTBrokerPort)))
        {
            Serial.println(F("MQTTBrokerPort failed to save"));
        }

        // ==== MQTTBrokerUsername ==== //
        if (!file.println(data.MQTTBrokerUsername))
        {
            Serial.println(F("MQTTBrokerUsername failed to save"));
        }

        // ==== MQTTBrokerPassword ==== //
        if (!file.println(data.MQTTBrokerPassword))
        {
            Serial.println(F("MQTTBrokerPassword failed to save"));
        }

        // ==== MQTTClientName ==== //
        if (!file.println(data.MQTTClientName))
        {
            Serial.println(F("MQTTClientName failed to save"));
        }

        // ==== isConfigured ==== //
        if (!file.println(String(data.isConfigured)))
        {
            Serial.println(F("isConfigured failed to save"));
        }
    }

    delay(this->FILE_LAST_WRITE_DELAY);
    file.close();
    this->configurationData = data;
    this->configurationDataReady = true;
};

/**
 * @brief Saves the settings data to the file on the filesystem
 * 
 * @param data The settings data to save
 */
void Filesystem::saveSettingsStripData(uint8_t stripID, FilesystemSettingsStripData data)
{

    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        Serial.println("Saving settings strip " + String(stripID + 1) + " data");
        Serial.println(F(""));

        File file = LittleFS.open("/" + this->settingsStripDataFilename[stripID], "w");
        if (!file)
        {
            Serial.println(F("Failed to open file for writing"));
            return;
        }
        else
        {
            file.write((byte *)&data, sizeof(data));
        }

        delay(this->FILE_LAST_WRITE_DELAY);
        file.close();
        this->settingsStripData[stripID] = data;
        this->settingsStripDataReady[stripID] = true;
    }
};

/**
 * @brief Saves the led state data to the file on the filesystem
 * 
 * @param data The led state data to save
 */
void Filesystem::saveLEDStripData(uint8_t stripID, FilesystemLEDStripData data)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        Serial.println("Saving led strip " + String(stripID + 1) + " data");
        Serial.println(F(""));

        File file = LittleFS.open("/" + this->ledStripDataFilename[stripID], "w");
        if (!file)
        {
            Serial.println(F("Failed to open file for writing"));
            return;
        }
        else
        {
            file.write((byte *)&data, sizeof(data));
        }

        delay(this->FILE_LAST_WRITE_DELAY);
        file.close();
        this->ledStripData[stripID] = data;
        this->ledStripDataReady[stripID] = true;
    }
};

/**
 * @brief Loads the motion data from the file on the filesystem
 * 
 * @return If the file exists the loaded motion data from the file
 */
FilesystemMotionData Filesystem::loadMotionData()
{
    FilesystemMotionData data;
    Serial.println(F("Loading motion data"));

    File file = LittleFS.open("/" + this->motionDataFilename, "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return data;
    }
    else
    {
        file.read((byte *)&data, sizeof(data));
    }

    file.close();
    this->motionData = data;
    this->motionDataReady = true;
    Serial.println(F("Loaded motion data"));
    this->parameterhandler->updateMotionParameter(data);
    return data;
};

/**
 * @brief Loads the configuration data from the file on the filesystem
 * 
 * @return If the file exists the loaded configuration data from the file
 */
FilesystemConfigurationData Filesystem::loadConfigurationData()
{
    FilesystemConfigurationData data;
    Serial.println(F("Loading configuration data"));

    /*
        We save and the load the data via the File.println because we are using dynamic types like string and that wont work with the byte read/write
    */

    File file = LittleFS.open("/" + this->configurationDataFilename, "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return data;
    }
    else
    {
        uint8_t state = 0;
        String message = "";
        char symbol = '\0';
        while (file.available())
        {
            int x = file.read();

            // Check for ascii symbol 13 => Carriage Return
            if (x == 13)
            {
                x = file.read();
                // Check for ascii symbol 10 => Line Feed
                if (x == 10)
                {
                    switch (state)
                    {
                        // ==== WiFiSSID ==== //
                    case 0:
                        data.WiFiSSID = message;
                        state++;
                        break;
                        // ==== WiFiPassword ==== //
                    case 1:
                        data.WiFiPassword = message;
                        state++;
                        break;
                        // ==== MQTTBrokerIpAddress ==== //
                    case 2:
                        data.MQTTBrokerIpAddress = message;
                        state++;
                        break;
                        // ==== MQTTBrokerPort ==== //
                    case 3:
                        data.MQTTBrokerPort = strtol(message.c_str(), NULL, 0);
                        state++;
                        break;
                        // ==== MQTTBrokerUsername ==== //
                    case 4:
                        data.MQTTBrokerUsername = message;
                        state++;
                        break;
                        // ==== MQTTBrokerPassword ==== //
                    case 5:
                        data.MQTTBrokerPassword = message;
                        state++;
                        break;
                        // ==== MQTTClientName ==== //
                    case 6:
                        data.MQTTClientName = message;
                        state++;
                        break;
                        // ==== isConfigured ==== //
                    case 7:
                        data.isConfigured = bool(message);
                        break;
                    }
                    message = "";
                }
            }
            else
            {
                if (x != 0)
                {
                    symbol = char(x);
                    message += String(symbol);
                }
            }
        }
    }

    file.close();
    this->configurationData = data;
    this->configurationDataReady = true;
    Serial.println(F("Loaded configuration data"));
    this->parameterhandler->updateConfigurationParameter(data);
    return data;
};

/**
 * @brief Loads the settings strip data from the file on the filesystem
 * 
 * @return If the file exists the loaded setttings strip data from the file
 */
FilesystemSettingsStripData Filesystem::loadSettingsStripData(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        FilesystemSettingsStripData data;
        Serial.println(("Loading settings strip " + String(stripID) + " data"));

        File file = LittleFS.open("/" + this->settingsStripDataFilename[stripID], "r");
        if (!file)
        {
            Serial.println(F("Failed to open file for reading"));
            return data;
        }
        else
        {
            file.read((byte *)&data, sizeof(data));
        }

        file.close();
        this->settingsStripData[stripID] = data;
        this->settingsStripDataReady[stripID] = true;
        Serial.println("Loaded settings strip " + String(stripID) + " data");
        this->parameterhandler->updateSettingsStripParameter(stripID, data);
        return data;
    }
};

/**
 * @brief Loads the led strip data from the file on the filesystem
 * 
 * @return If the file exists the loaded led strip data from the file
 */
FilesystemLEDStripData Filesystem::loadLEDStripData(uint8_t stripID)
{

    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        FilesystemLEDStripData data;
        Serial.println(("Loading led strip " + String(stripID) + " data"));

        File file = LittleFS.open("/" + this->ledStripDataFilename[stripID], "r");
        if (!file)
        {
            Serial.println(F("Failed to open file for reading"));
            return data;
        }
        else
        {
            file.read((byte *)&data, sizeof(data));
        }

        file.close();
        this->ledStripData[stripID] = data;
        this->ledStripDataReady[stripID] = true;
        Serial.println("Loaded led strip " + String(stripID) + " data");
        this->parameterhandler->updateLEDStripParameter(stripID, data);
        return data;
    }
};

/**
 * @brief Creates a file on the filesystem if its missing
 * 
 * @param filename The name of the file to create
 */
void Filesystem::createFileIfMissing(String filename)
{
    if (!LittleFS.exists("/" + filename))
    {
        Serial.println("Creating missing file: '" + filename + "'");
        File file = LittleFS.open("/" + filename, "w+");
        file.close();
    }
    else
    {
        Serial.println("Found file: '" + filename + "'");
    }
}

/**
 * @brief Resets the motion file on the filesystem and the motion data
 * 
 */
void Filesystem::resetMotionData()
{
    this->resetFileIfExists(this->motionDataFilename);
    this->motionData = {};
    this->motionDataReady = false;
}

/**
 * 
 * @return True the motion data is ready (loaded / saved)
 */
bool Filesystem::isMotionDataReady()
{
    return this->motionDataReady;
}

/**
 * @brief Resets the configuration file on the filesystem and the configuration data
 * 
 */
void Filesystem::resetConfigurationData()
{
    this->resetFileIfExists(this->configurationDataFilename);
    this->configurationData = {};
    this->configurationDataReady = false;
}

/**
 * 
 * @return True the configuration data is ready (loaded / saved)
 */
bool Filesystem::isConfigurationDataReady()
{
    return this->configurationDataReady;
}

/**
 * @brief Resets the settings file on the filesystem and the settings data
 * 
 */
void Filesystem::resetSettingsStripData(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->resetFileIfExists(this->settingsStripDataFilename[stripID]);
        this->settingsStripData[stripID] = {};
        this->settingsStripDataReady[stripID] = false;
    }
}

/**
 * 
 * @return True the settings data is ready (loaded / saved)
 */
bool Filesystem::isSettingsStripDataReady(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        return this->settingsStripDataReady[stripID];
    }
}

/**
 * @brief Resets the led strip data file on the filesystem and the led strip data
 * 
 */
void Filesystem::resetLEDStripData(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        this->resetFileIfExists(this->ledStripDataFilename[stripID]);
        this->ledStripData[stripID] = {};
        this->ledStripDataReady[stripID] = false;
    }
}

/**
 * 
 * @return True the led strip data is ready (loaded / saved)
 */
bool Filesystem::isLEDStripDataReady(uint8_t stripID)
{
    if (stripID >= 0 && stripID < STRIP_COUNT)
    {
        return this->ledStripDataReady[stripID];
    }
}

/**
 * @brief Resets a file on the filesystem if it exists
 * 
 * @param filename The name of the file to reset
 */
void Filesystem::resetFileIfExists(String filename)
{
    if (LittleFS.exists(filename))
    {
        Serial.println("Resetting file: '" + filename + "'");
        File file = LittleFS.open("/" + filename, "w");
        file.close();
    }
};

/**
 * @brief Lists all found files in the Filesystem with its size
 * 
 */
void Filesystem::listAllFiles()
{
    Serial.println(F("Found following files in root dir:"));

    Dir root = LittleFS.openDir("/");

    while (root.next())
    {
        File file = root.openFile("r");
        Serial.print(F("  FILE: "));
        Serial.print(root.fileName());
        Serial.print(F("  SIZE: "));
        Serial.println(file.size());
        file.close();
    }
};
