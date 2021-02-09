#include "OTA.h"

/**
 * Empty constructor
 */
OTA::OTA(){

};

/**
 * Sets reference to external components
 */
void OTA::setReference(Network *network,
                       Configuration *configuration)
{
    this->network = network;
    this->configuration = configuration;
};

/**
 * Does init stuff for the OTA component
 * 
 * @return True if successfull, false if not 
 */
bool OTA::Init()
{
    if (!init)
    {

        if (network->wifiConnected && configuration->isFinished)
        {
            ArduinoOTA.setHostname(configuration->data.mqttClientName.c_str());
            ArduinoOTA.setPassword(configuration->data.mqttBrokerPassword.c_str());

            ArduinoOTA.onStart([]() {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
                {
                    type = "sketch";
                }
                else
                { // U_FS
                    type = "filesystem";
                }

                // NOTE: if updating FS this would be the place to unmount FS using FS.end()
                Serial.println("Start updating " + type);
            });

            ArduinoOTA.onEnd([]() {
                Serial.println(F("\nEnd"));
            });

            ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
                Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
            });

            ArduinoOTA.onError([](ota_error_t error) {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR)
                {
                    Serial.println(F("Auth Failed"));
                }
                else if (error == OTA_BEGIN_ERROR)
                {
                    Serial.println(F("Begin Failed"));
                }
                else if (error == OTA_CONNECT_ERROR)
                {
                    Serial.println(F("Connect Failed"));
                }
                else if (error == OTA_RECEIVE_ERROR)
                {
                    Serial.println(F("Receive Failed"));
                }
                else if (error == OTA_END_ERROR)
                {
                    Serial.println(F("End Failed"));
                }
            });

            ArduinoOTA.begin();

            init = true;
        }
    }

    return init;
};

/**
 * Runs the OTA component
 */
void OTA::Run()
{
    if (!init)
    {
        Init();
        return;
    }
    ArduinoOTA.handle();
};
