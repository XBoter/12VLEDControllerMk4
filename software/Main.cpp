#include "Main.h"

// Global instace of the mainController
Main mainController;

/**
 *  Empty constructor
 */
Main::Main()
{
}

/**
 * Setup function for Arduino file to call in setup
 */
void Main::_setup()
{
    // Setup Serial
    Serial.begin(BAUDRATE);
    delay(100);

    // Info and Version print
    Serial.println("# ======================== #");
    Serial.print("Name          : ");
    Serial.println(Name);
    Serial.print("Programmer    : ");
    Serial.println(Programmer);
    Serial.print("Created       : ");
    Serial.println(Created);
    Serial.print("Last Modifed  : ");
    Serial.println(LastModifed);
    Serial.print("Version       : ");
    Serial.println(Version);
    Serial.println("# ======================== #");
    Serial.println("");

    // Init all configuration
    configuration.Init();
};

/**
 * Loop function for Arduino file to call in loop
 */
void Main::_loop()
{
    //  Messure loop time
    /*
    unsigned long curMicros = micros();
    Serial.println("Loop time : " + String(curMicros - PrevMicros_Loop));
    PrevMicros_Loop = curMicros;
    */

    // Run configuration before all other components
    configuration.Run();

    if (configuration.isFinished)
    {
        switch (state)
        {
        case 0:
            // Init all components
            otaGit.Init();
            i2c.Init();
            network.Init();
            //powerMessurement.Init();
            ledDriver.Init();
            information.Init();
            pirReader.Init();
            state++;
            break;

        case 1:
            // Run all components
            otaGit.Run();
            i2c.Run();
            network.Run();
            //powerMessurement.Run();
            ledDriver.Run();
            information.Run();
            pirReader.Run();

            // Reset virtual pir sensor at end of current loop after all components got called
            if(network.virtualPIRSensorTriggered)
            {
                network.virtualPIRSensorTriggered = false;
            }

            break;
        }
    }
    else
    {
        i2c.init = false;
        network.init = false;
        powerMessurement.init = false;
        ledDriver.init = false;
        information.init = false;
        pirReader.init = false;
        state = 0;
    }
};
