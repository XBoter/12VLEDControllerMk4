#include "Main.h"

// Global instace of the mainController
Main mainController;

using namespace LedControllerSoftwareMk5;

Main::Main()
{

};


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


    // Init
    i2c.Init();
    network.Init();
    powerMessurement.Init();
    ledDriver.Init();
    information.Init();
    pirReader.Init();
};


void Main::_loop()
{
    // Run 
    i2c.Run();
    network.Run();
    powerMessurement.Run();
    ledDriver.Run();
    information.Run();
    pirReader.Run();
};

