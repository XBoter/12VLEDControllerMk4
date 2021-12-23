#include "LEDControllerMk4.h"

LEDControllerMk4::LEDControllerMk4()
{
}

/**
 * Setup function for Arduino file to call in setup
 */
void LEDControllerMk4::_setup()
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

    // Set references to external components
    ota.setReference(&network,
                     &configuration);

    i2c.setReference();

    network.setReference(&configuration,
                         &information,
                         &pirReader,
                         &powerMessurement);

    powerMessurement.setReference(&i2c,
                                  &network);

    ledDriver.setReference(&i2c,
                           &network,
                           &pirReader);

    information.setReference(&network,
                             &memNetwork,
                             &pirReader,
                             &memPirReader);

    pirReader.setReference(&network);

    webserver.setReference(&ledDriver);

    // Init configuration
    webserver.Init();

    Serial.println("# ==== Setup finished ==== #");
    Serial.println("");
};

/**
 * Loop function for Arduino file to call in loop
 */
void LEDControllerMk4::_loop()
{

    unsigned long x = 0;
    unsigned long y = 0;

    y = micros();
    x = micros();

    // Run configuration before all other components
    webserver.Run();
    yield();

    difTimeConfiguration[cycleCounter] = micros() - x;

    if (webserver.isFinished)
    {
        switch (state)
        {
        case 0:
            // Reset init flag
            ota.init = false;
            i2c.init = false;
            network.init = false;
            powerMessurement.init = false;
            ledDriver.init = false;
            information.init = false;
            pirReader.init = false;
            state++;
            break;

        case 1:
            // Init all components
            ota.Init();
            i2c.Init();
            network.Init();
            //powerMessurement.Init();
            ledDriver.Init();
            information.Init();
            pirReader.Init();
            state++;
            break;

        case 2:
            // Run all components
            // == OTA
            x = micros();
            ota.Run();
            yield(); 
            difTimeOTA[cycleCounter] = micros() - x;

            // == I2C
            x = micros();
            i2c.Run();
            yield(); 
            difTimeI2C[cycleCounter] = micros() - x;

            // == Network
            x = micros();
            network.Run();
            yield(); 
            difTimeNetwork[cycleCounter] = micros() - x;

            // == PowerMeasurement
            x = micros();
            //powerMessurement.Run();
            //yield(); 
            difTimePowerMeasurement[cycleCounter] = micros() - x;

            // == LED
            x = micros();
            ledDriver.Run();
            yield(); 
            difTimeLED[cycleCounter] = micros() - x;

            // == Information
            x = micros();
            information.Run();
            yield(); 
            difTimeInformation[cycleCounter] = micros() - x;

            // == PIR 
            x = micros();
            pirReader.Run();
            yield(); 
            difTimePIR[cycleCounter] = micros() - x;

            // Reset virtual pir sensor at end of current loop after all components got called
            if (network.virtualPIRSensorTriggered)
            {
                network.virtualPIRSensorTriggered = false;
            }

            break;
        }
    }
    else
    {
        state = 0;
    }

    if(enablePerformanceMonitor)
    {
        difTimeAll[cycleCounter] = micros() - y;
        cycleCounter++;
        if(cycleCounter >= cycle)
        {
            CalcPerformance();
            cycleCounter = 0;
        }
    }

};


/**
 * Calc performance times for analysis
 */
void LEDControllerMk4::CalcPerformance()
{

    unsigned long avgTimeI2C = 0;
    unsigned long avgTimeWebserver = 0;
    unsigned long avgTimeNetwork = 0;
    unsigned long avgTimeOTA = 0;
    unsigned long avgTimePowerMeasurement = 0;
    unsigned long avgTimePIR = 0;
    unsigned long avgTimeLED = 0;
    unsigned long avgTimeInformation = 0;
    unsigned long avgTimeAll = 0;

    double percent = 0.0;

    for(int i = 0; i < cycle - 1; i++)
    {
      avgTimeI2C += difTimeI2C[i];
      avgTimeWebserver += difTimeWebserver[i];
      avgTimeNetwork += difTimeNetwork[i];
      avgTimeOTA += difTimeOTA[i];
      avgTimePowerMeasurement += difTimePowerMeasurement[i];
      avgTimePIR += difTimePIR[i];
      avgTimeLED += difTimeLED[i];
      avgTimeInformation += difTimeInformation[i];
      avgTimeAll += difTimeAll[i];
    }

    avgTimeI2C = avgTimeI2C / localCycle;
    avgTimeWebserver = avgTimeWebserver / localCycle;
    avgTimeNetwork = avgTimeNetwork / localCycle;
    avgTimeOTA = avgTimeOTA / localCycle;
    avgTimePowerMeasurement = avgTimePowerMeasurement / localCycle;
    avgTimePIR = avgTimePIR / localCycle;
    avgTimeLED = avgTimeLED / localCycle;
    avgTimeInformation = avgTimeInformation / localCycle;
    avgTimeAll = avgTimeAll / localCycle;

    if(avgTimeAll == 0){
        return;
    }

    information.TopSpacerPrint();

    // ==== I2C
    percent = double(avgTimeI2C) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("I2C Time        : "));
    Serial.println(avgTimeI2C);
    Serial.print(F("I2C Percent     : "));
    Serial.println(percent);

    // ==== Configuration
    percent = double(avgTimeWebserver) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("Webserver Time     : "));
    Serial.println(avgTimeWebserver);
    Serial.print(F("Webserver Percent  : "));
    Serial.println(percent);

    // ==== Network
    percent = double(avgTimeNetwork) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("Network Time    : "));
    Serial.println(avgTimeNetwork);
    Serial.print(F("Network Percent : "));
    Serial.println(percent);

    // ==== OTA
    percent = double(avgTimeOTA) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("OTA Time        : "));
    Serial.println(avgTimeOTA);
    Serial.print(F("OTA Percent     : "));
    Serial.println(percent);

    // ==== Power
    percent = double(avgTimePowerMeasurement) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("Power Time      : "));
    Serial.println(avgTimePowerMeasurement);
    Serial.print(F("Power Percent   : "));
    Serial.println(percent);

    // ==== PIR
    percent = double(avgTimePIR) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("PIR Time        : "));
    Serial.println(avgTimePIR);
    Serial.print(F("PIR Percent     : "));
    Serial.println(percent);

    // ==== LED
    percent = double(avgTimeLED) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("LED Time        : "));
    Serial.println(avgTimeLED);
    Serial.print(F("LED Percent     : "));
    Serial.println(percent);

    // ==== Info
    percent = double(avgTimeInformation) / double(avgTimeAll) * 100;
    information.InsertPrint();
    Serial.print(F("Info Time       : "));
    Serial.println(avgTimeInformation);
    Serial.print(F("Info Percent    : "));
    Serial.println(percent);


    // ==== All
    information.InsertPrint();
    Serial.print(F("All Time        : "));
    Serial.println(avgTimeAll);

    information.BottomSpacerPrint();

}