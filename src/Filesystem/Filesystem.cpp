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
        }

        // == Create files if missing
        this->createFileIfMissing(this->configurationFilename);
        this->createFileIfMissing(this->settingsFilename);
        this->createFileIfMissing(this->ledStateFilename);

        // == List all found files in LittleFS
        this->listAllFiles();

        // == Load all files
        this->loadConfiguration();
        this->loadSettings();
        this->loadLEDState();

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

    // If the settings data is not configured once we save the default settings to the filesystem
    if (this->isSettingsDataReady())
    {
        if (!this->getSettingData().isConfigured)
        {
            // Inital Output config
            this->settingsData.stripChannelOutputs[0][0] = LEDOutputType::CW;
            this->settingsData.stripChannelOutputs[0][1] = LEDOutputType::B;
            this->settingsData.stripChannelOutputs[0][2] = LEDOutputType::R;
            this->settingsData.stripChannelOutputs[0][3] = LEDOutputType::G;
            this->settingsData.stripChannelOutputs[0][4] = LEDOutputType::WW;

            this->settingsData.stripChannelOutputs[1][0] = LEDOutputType::CW;
            this->settingsData.stripChannelOutputs[1][1] = LEDOutputType::B;
            this->settingsData.stripChannelOutputs[1][2] = LEDOutputType::R;
            this->settingsData.stripChannelOutputs[1][3] = LEDOutputType::G;
            this->settingsData.stripChannelOutputs[1][4] = LEDOutputType::WW;

            this->settingsData.isConfigured = true;
            this->saveSettings(this->settingsData);
        }
    }

    // If the led state data is not configured once we save the default data to the filesystem
    if (this->isLEDStateDataReady())
    {
        if (!this->getLEDStateData().isConfigured)
        {
            // Inital led state config
            this->ledStateData.Power[0] = false;
            this->ledStateData.RedValue[0] = 0;
            this->ledStateData.GreenValue[0] = 0;
            this->ledStateData.BlueValue[0] = 0;
            this->ledStateData.ColdWhiteValue[0] = 0;
            this->ledStateData.WarmWhiteValue[0] = 0;
            this->ledStateData.BrightnessValue[0] = 0;
            this->ledStateData.EffectValue[0] = SingleLEDEffect::None;

            this->ledStateData.Power[1] = false;
            this->ledStateData.RedValue[1] = 0;
            this->ledStateData.GreenValue[1] = 0;
            this->ledStateData.BlueValue[1] = 0;
            this->ledStateData.ColdWhiteValue[1] = 0;
            this->ledStateData.WarmWhiteValue[1] = 0;
            this->ledStateData.BrightnessValue[1] = 0;
            this->ledStateData.EffectValue[1] = SingleLEDEffect::None;

            this->ledStateData.isConfigured = true;
            this->saveLEDState(this->ledStateData);
        }
    }
};

/**
 * @brief Returns the loaded ConfigurationData if 'configurationDataReady' is true
 * 
 * @return The loaded ConfigurationData from the filesystem
 */
ConfigurationData Filesystem::getConfigurationData()
{
    if (this->configurationDataReady)
    {
        return this->configurationData;
    }
};

/**
 * @brief Returns the loaded SettingsData if 'settingsDataReady' is true
 * 
 * @return The loaded SettingsData from the filesystem
 */
SettingsData Filesystem::getSettingData()
{
    if (this->settingsDataReady)
    {
        return this->settingsData;
    }
};

/**
 * @brief Returns the loaded LEDStateData if 'ledStateDataReady' is true
 * 
 * @return The loaded LEDStateData from the filesystem
 */
LEDStateData Filesystem::getLEDStateData()
{
    if (this->ledStateDataReady)
    {
        return this->ledStateData;
    }
};

/**
 * @brief Saves the configuration data to the file on the filesystem
 * 
 * @param data The configuration data to save
 */
void Filesystem::saveConfiguration(ConfigurationData data)
{
    Serial.println(F("Saving configuration data"));
    Serial.println(F(""));

    File file = LittleFS.open("/" + this->configurationFilename, "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    // ==== wifiSSID ==== //
    if (!file.println(data.wifiSSID))
    {
        Serial.println(F("wifiSSID failed to save"));
    }

    // ==== wifiPassword ==== //
    if (!file.println(data.wifiPassword))
    {
        Serial.println(F("wifiPassword failed to save"));
    }

    // ==== mqttBrokerIpAddress ==== //
    if (!file.println(data.mqttBrokerIpAddress))
    {
        Serial.println(F("mqttBrokerIpAddress failed to save"));
    }

    // ==== mqttBrokerPort ==== //
    if (!file.println(String(data.mqttBrokerPort)))
    {
        Serial.println(F("mqttBrokerPort failed to save"));
    }

    // ==== mqttBrokerUsername ==== //
    if (!file.println(data.mqttBrokerUsername))
    {
        Serial.println(F("mqttBrokerUsername failed to save"));
    }

    // ==== mqttBrokerPassword ==== //
    if (!file.println(data.mqttBrokerPassword))
    {
        Serial.println(F("mqttBrokerPassword failed to save"));
    }

    // ==== mqttClientName ==== //
    if (!file.println(data.mqttClientName))
    {
        Serial.println(F("mqttClientName failed to save"));
    }

    // ==== isConfigured ==== //
    if (!file.println(String(data.isConfigured)))
    {
        Serial.println(F("isConfigured failed to save"));
    }

    delay(2000); // Make sure the CREATE and LASTWRITE times are different
    file.close();
    this->configurationData = data;
    this->configurationDataReady = true;
};

/**
 * @brief Saves the settings data to the file on the filesystem
 * 
 * @param data The settings data to save
 */
void Filesystem::saveSettings(SettingsData data)
{
    Serial.println(F("Saving settings data"));
    Serial.println(F(""));

    File file = LittleFS.open("/" + this->settingsFilename, "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    // ==== stripChannelOutputs ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        for (int j = 0; j < CHANNEL_COUNT; j++)
        {
            if (!file.println(String(this->helper->LEDOutputTypeToUint8(data.stripChannelOutputs[i][j]))))
            {
                Serial.println(F("Settings stripChannelOutputs failed to save"));
            }
        }
    }

    // ==== isConfigured ==== //
    if (!file.println(String(data.isConfigured)))
    {
        Serial.println(F("Settings isConfigured failed to save"));
    }

    delay(2000); // Make sure the CREATE and LASTWRITE times are different
    file.close();
    this->settingsData = data;
    this->settingsDataReady = true;
};

/**
 * @brief Saves the led state data to the file on the filesystem
 * 
 * @param data The led state data to save
 */
void Filesystem::saveLEDState(LEDStateData data)
{
    Serial.println(F("Saving led state data"));
    Serial.println(F(""));

    File file = LittleFS.open("/" + this->ledStateFilename, "w");
    if (!file)
    {
        Serial.println(F("Failed to open file for writing"));
        return;
    }

    // ==== Power ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.Power[i])))
        {
            Serial.println("Strip " + String(i + 1) + " Power failed to save");
        }
    }

    // ==== RedValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.RedValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " RedValue failed to save");
        }
    }

    // ==== GreenValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.GreenValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " GreenValue failed to save");
        }
    }

    // ==== BlueValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.BlueValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " BlueValue failed to save");
        }
    }

    // ==== ColdWhiteValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.ColdWhiteValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " ColdWhiteValue failed to save");
        }
    }

    // ==== WarmWhiteValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.WarmWhiteValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " WarmWhiteValue failed to save");
        }
    }

    // ==== BrightnessValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(String(data.BrightnessValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " BrightnessValue failed to save");
        }
    }

    // ==== EffectValue ==== //
    for (int i = 0; i < STRIP_COUNT; i++)
    {
        if (!file.println(this->helper->SingleLEDEffectToString(data.EffectValue[i])))
        {
            Serial.println("Strip " + String(i + 1) + " EffectValue failed to save");
        }
    }

    // ==== isConfigured ==== //
    if (!file.println(String(data.isConfigured)))
    {
        Serial.println(F("isConfigured failed to save"));
    }

    delay(2000); // Make sure the CREATE and LASTWRITE times are different
    file.close();
    this->ledStateData = data;
    this->ledStateDataReady = true;
};

/**
 * @brief Loads the configuration data from the file on the filesystem
 * 
 * @return If the file exists the loaded configuration data from the file
 */
ConfigurationData Filesystem::loadConfiguration()
{
    ConfigurationData data;
    Serial.println(F("Loading configuration data"));

    File file = LittleFS.open("/" + this->configurationFilename, "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return data;
    }

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
                    // ==== wifiSSID ==== //
                case 0:
                    data.wifiSSID = message;
                    state++;
                    break;
                    // ==== wifiPassword ==== //
                case 1:
                    data.wifiPassword = message;
                    state++;
                    break;
                    // ==== mqttBrokerIpAddress ==== //
                case 2:
                    data.mqttBrokerIpAddress = message;
                    state++;
                    break;
                    // ==== mqttBrokerPort ==== //
                case 3:
                    data.mqttBrokerPort = strtol(message.c_str(), NULL, 0);
                    state++;
                    break;
                    // ==== mqttBrokerUsername ==== //
                case 4:
                    data.mqttBrokerUsername = message;
                    state++;
                    break;
                    // ==== mqttBrokerPassword ==== //
                case 5:
                    data.mqttBrokerPassword = message;
                    state++;
                    break;
                    // ==== mqttClientName ==== //
                case 6:
                    data.mqttClientName = message;
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
    file.close();
    this->configurationData = data;
    this->configurationDataReady = true;
    Serial.println(F("Loaded configuration data"));
    return data;
};

/**
 * @brief Loads the settings data from the file on the filesystem
 * 
 * @return If the file exists the loaded setttings data from the file
 */
SettingsData Filesystem::loadSettings()
{
    SettingsData data;
    Serial.println(F("Loading settings data"));

    File file = LittleFS.open("/" + this->settingsFilename, "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return data;
    }

    uint8_t state = 0;
    uint8_t i = 0;
    uint8_t j = 0;
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
                    // ==== stripChannelOutputs ==== //
                case 0:
                    data.stripChannelOutputs[i][j] = this->helper->Uint8ToLEDOutputType(strtol(message.c_str(), NULL, 0));
                    Serial.println("Strip " + String(i + 1) + " Channel " + String(j + 1) + ": " + String(this->helper->LEDOutputTypeToUint8(data.stripChannelOutputs[i][j])));
                    j++;
                    if (j >= CHANNEL_COUNT)
                    {
                        j = 0;
                        i++;
                    }
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        j = 0;
                        state++;
                    }
                    break;

                    // ==== isConfigured ==== //
                case 1:
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
    file.close();
    this->settingsData = data;
    this->settingsDataReady = true;
    Serial.println(F("Loaded settings data"));
    return data;
};

/**
 * @brief Loads the led state data from the file on the filesystem
 * 
 * @return If the file exists the loaded led state data from the file
 */
LEDStateData Filesystem::loadLEDState()
{
    LEDStateData data;
    Serial.println(F("Loading led state data"));

    File file = LittleFS.open("/" + this->ledStateFilename, "r");
    if (!file)
    {
        Serial.println(F("Failed to open file for reading"));
        return data;
    }

    uint8_t state = 0;
    uint8_t i = 0;
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
                    // ==== Power ==== //
                case 0:
                    data.Power[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " Power : " + String(data.Power[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== RedValue ==== //
                case 1:
                    data.RedValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " RedValue : " + String(data.RedValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== GreenValue ==== //
                case 2:
                    data.GreenValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " GreenValue : " + String(data.GreenValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== BlueValue ==== //
                case 3:
                    data.BlueValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " BlueValue : " + String(data.BlueValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== ColdWhiteValue ==== //
                case 4:
                    data.ColdWhiteValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " ColdWhiteValue : " + String(data.ColdWhiteValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== WarmWhiteValue ==== //
                case 5:
                    data.WarmWhiteValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " WarmWhiteValue : " + String(data.WarmWhiteValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== BrightnessValue ==== //
                case 6:
                    data.BrightnessValue[i] = bool(message);
                    Serial.println("Strip " + String(i + 1) + " BrightnessValue : " + String(data.BrightnessValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;
                    // ==== EffectValue ==== //
                case 7:
                    data.EffectValue[i] = this->helper->Uint8ToSingleLEDEffect(strtol(message.c_str(), NULL, 0));
                    Serial.println("Strip " + String(i + 1) + " EffectValue : " + this->helper->SingleLEDEffectToString(data.EffectValue[i]));
                    i++;
                    if (i >= STRIP_COUNT)
                    {
                        i = 0;
                        state++;
                    }
                    break;

                    // ==== isConfigured ==== //
                case 8:
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
    file.close();
    this->ledStateData = data;
    this->ledStateDataReady = true;
    Serial.println(F("Loaded led state data"));
    return data;
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
 * @brief Resets the configuration file on the filesystem and the configuration data
 * 
 */
void Filesystem::resetConfiguration()
{
    this->resetFileIfExists(this->configurationFilename);
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
 * 
 * @return True the settings data is ready (loaded / saved)
 */
bool Filesystem::isSettingsDataReady()
{
    return this->settingsDataReady;
}

/**
 * 
 * @return True the led state data is ready (loaded / saved)
 */
bool Filesystem::isLEDStateDataReady()
{
    return this->ledStateDataReady;
}

/**
 * @brief Resets the settings file on the filesystem and the settings data
 * 
 */
void Filesystem::resetSettings()
{
    this->resetFileIfExists(this->settingsFilename);
    this->settingsData = {};
    this->settingsDataReady = false;
}

/**
 * @brief Resets the led state file on the filesystem and the led state data
 * 
 */
void Filesystem::resetLEDState()
{
    this->resetFileIfExists(this->ledStateFilename);
    this->ledStateData = {};
    this->ledStateDataReady = false;
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
