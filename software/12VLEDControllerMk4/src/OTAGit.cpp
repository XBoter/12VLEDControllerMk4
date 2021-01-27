#include "../include/OTAGit.h"

/**
 * Empty constructor
 */
OTAGit::OTAGit(Network *network)
{
    this->network = network;
};

/**
 * Does init stuff for the OTAGit component
 * 
 * @return True if successfull, false if not 
 */
bool OTAGit::Init()
{
    if (!init)
    {
        if (network->wifiConnected)
        {

            // Search for certs on LittleFS
            int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
            Serial.print(F("Number of CA certs read: "));
            Serial.println(numCerts);
            if (numCerts == 0)
            {
                // If no certs a found we cant upgrade 
                Serial.println(F("No certs found. Did you upload the certs.ar file to the LittleFS directory before running?"));
            }
            else
            {
                foundCertificates = true;

                // Initial check for updates after reboot or reconfiguration
                HandleUpgrade();
            }

            init = true;
        }
    }

    return init;
};

/**
 * Runs the OTAGit component
 */
void OTAGit::Run()
{
    if (!init)
    {
        Init();
        return;
    }

    // We only check for updates if we got certs
    if (foundCertificates && network->wifiConnected)
    {
        // Check for upgrade at 03:00
        if (false)
        {
            HandleUpgrade();
        }
    }
};

/**
 * Searches on GitHub for a new version and upgrades the sketch
 */
void OTAGit::HandleUpgrade()
{
    //We do this locally so that the memory used is freed when the function exists.
    ESPOTAGitHub ESPOTAGitHub(&certStore, GHOTA_USER, GHOTA_REPO, GHOTA_CURRENT_TAG, GHOTA_BIN_FILE, GHOTA_ACCEPT_PRERELEASE);

    Serial.println("")
    Serial.println("Checking for update...");
    if (ESPOTAGitHub.checkUpgrade())
    {
        Serial.print("Upgrade found at: ");
        Serial.println(ESPOTAGitHub.getUpgradeURL());
        if (ESPOTAGitHub.doUpgrade())
        {
            Serial.println("Upgrade complete."); //This should never be seen as the device should restart on successful upgrade.
        }
        else
        {
            Serial.print("Unable to upgrade: ");
            Serial.println(ESPOTAGitHub.getLastError());
        }
    }
    else
    {
        Serial.print("Not proceeding to upgrade: ");
        Serial.println(ESPOTAGitHub.getLastError());
    }
}