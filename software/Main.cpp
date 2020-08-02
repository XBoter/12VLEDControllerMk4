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

    // Init all components
    i2c.Init();
    network.Init();
    powerMessurement.Init();
    ledDriver.Init();
    information.Init();
    pirReader.Init();
};


/**
 * Loop function for Arduino file to call in loop
 */
void Main::_loop()
{
    // Run all components
    i2c.Run();
    network.Run();
    powerMessurement.Run();
    ledDriver.Run();
    information.Run();
    pirReader.Run();
};
