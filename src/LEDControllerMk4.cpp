#include "LEDControllerMk4.h"

LEDControllerMk4::LEDControllerMk4()
{
}
/**
 * @brief Setup function call in Arduino Main
 * 
 */
void LEDControllerMk4::_setup()
{
    Serial.begin(BAUDRATE);
    delay(100);

    // ================ Initial Info Print ================ //
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

    // ================ Component references ================ //
    this->ota.setReference(&this->network,
                           &this->filesystem);
    this->i2c.setReference();
    this->network.setReference(&this->filesystem,
                               &this->helper,
                               &this->information,
                               &this->pirReader,
                               &this->powerMessurement,
                               &this->parameterhandler);
    this->powerMessurement.setReference(&this->i2c,
                                        &this->network);
    this->ledDriver.setReference(&this->i2c,
                                 &this->network,
                                 &this->pirReader,
                                 &this->filesystem);
    this->information.setReference(&this->helper);
    this->pirReader.setReference(&this->network,
                                 &this->information,
                                 &this->helper);
    this->webserver.setReference(&this->filesystem,
                                 &this->helper,
                                 &this->network,
                                 &this->parameterhandler);
    this->helper.setReference();
    this->filesystem.setReference(&this->helper,
                                  &this->parameterhandler);
    this->parameterhandler.setReference();

    Serial.println("# ==== Setup finished ==== #");
    Serial.println("");
};

/**
 * @brief Loop function call in Arduino Main
 * 
 */
void LEDControllerMk4::_loop()
{

    unsigned long x = 0;
    unsigned long y = 0;

    y = micros();
    x = micros();

    yield();

    switch (state)
    {
    // ================ Reset ================ //
    case 0:
        this->ota.init = false;
        this->i2c.init = false;
        this->network.init = false;
        //this->powerMessurement.init = false;
        this->ledDriver.init = false;
        this->information.init = false;
        this->pirReader.init = false;
        this->webserver.init = false;
        this->filesystem.init = false;
        this->helper.init = false;
        this->parameterhandler.init = false;
        state++;
        break;

    // ================ Call Components Init Function ================ //
    case 1:
        this->ota.Init();
        this->i2c.Init();
        this->network.Init();
        //this->powerMessurement.Init();
        this->ledDriver.Init();
        this->information.Init();
        this->pirReader.Init();
        this->filesystem.Init();
        this->webserver.Init();
        this->helper.Init();
        this->parameterhandler.Init();
        state++;
        break;

    // ================ Call Components Run Function ================ //
    case 2:
        // ======== OTA ======== //
        x = micros();
        this->ota.Run();
        yield();
        difTimeOTA[cycleCounter] = micros() - x;
        // ======== I2C ======== //
        x = micros();
        this->i2c.Run();
        yield();
        difTimeI2C[cycleCounter] = micros() - x;
        // ======== NETWORK ======== //
        x = micros();
        this->network.Run();
        yield();
        difTimeNetwork[cycleCounter] = micros() - x;
        // ======== POWER MESSUREMENT ======== //
        x = micros();
        //this->powerMessurement.Run();
        //yield();
        difTimePowerMeasurement[cycleCounter] = micros() - x;
        // ======== I2C ======== //
        x = micros();
        this->ledDriver.Run();
        yield();
        difTimeLED[cycleCounter] = micros() - x;
        // ======== INFORMATION ======== //
        x = micros();
        this->information.Run();
        yield();
        difTimeInformation[cycleCounter] = micros() - x;
        // ======== PIR ======== //
        x = micros();
        this->pirReader.Run();
        yield();
        difTimePIR[cycleCounter] = micros() - x;
        // ======== WEBSERVER ======== //
        x = micros();
        this->webserver.Run();
        yield();
        difTimeWebserver[cycleCounter] = micros() - x;
        // ======== HELPER ======== //
        x = micros();
        this->helper.Run();
        yield();
        difTimeHelper[cycleCounter] = micros() - x;
        // ======== FILESYSTEM ======== //
        x = micros();
        this->filesystem.Run();
        yield();
        difTimeFilesystem[cycleCounter] = micros() - x;
        // ======== PARAMETERHANDLER ======== //
        x = micros();
        this->parameterhandler.Run();
        yield();
        difTimeParameterhandler[cycleCounter] = micros() - x;
        // Reset virtual pir sensor at end of current loop after all components got called
        network.resetVirtualPIRSensor();
        break;
    }

    if (enablePerformanceMonitor)
    {
        difTimeAll[cycleCounter] = micros() - y;
        cycleCounter++;
        if (cycleCounter >= cycle)
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
    unsigned long avgTimeFilesystem = 0;
    unsigned long avgTimeHelper = 0;
    unsigned long avgTimeParameterhandler = 0;
    unsigned long avgTimeAll = 0;

    double percent = 0.0;

    for (int i = 0; i < cycle - 1; i++)
    {
        avgTimeI2C += difTimeI2C[i];
        avgTimeWebserver += difTimeWebserver[i];
        avgTimeNetwork += difTimeNetwork[i];
        avgTimeOTA += difTimeOTA[i];
        avgTimePowerMeasurement += difTimePowerMeasurement[i];
        avgTimePIR += difTimePIR[i];
        avgTimeLED += difTimeLED[i];
        avgTimeInformation += difTimeInformation[i];
        avgTimeFilesystem += difTimeFilesystem[i];
        avgTimeHelper += difTimeHelper[i];
        avgTimeParameterhandler += difTimeParameterhandler[i];
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
    avgTimeFilesystem = avgTimeFilesystem / localCycle;
    avgTimeHelper = avgTimeHelper / localCycle;
    avgTimeParameterhandler = avgTimeParameterhandler / localCycle;
    avgTimeAll = avgTimeAll / localCycle;

    if (avgTimeAll == 0)
    {
        return;
    }

    helper.TopSpacerPrint();

    // ================ I2C ================ //
    percent = double(avgTimeI2C) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("I2C Time                    : "));
    Serial.println(avgTimeI2C);
    Serial.print(F("I2C Percent                 : "));
    Serial.println(percent);
    // ============ WEBSERVER ================ //
    percent = double(avgTimeWebserver) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Webserver Time              : "));
    Serial.println(avgTimeWebserver);
    Serial.print(F("Webserver Percent           : "));
    Serial.println(percent);
    // ============ NETWORK ================ //
    percent = double(avgTimeNetwork) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Network Time                : "));
    Serial.println(avgTimeNetwork);
    Serial.print(F("Network Percent             : "));
    Serial.println(percent);
    // ============ OTA ================ //
    percent = double(avgTimeOTA) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("OTA Time                    : "));
    Serial.println(avgTimeOTA);
    Serial.print(F("OTA Percent                 : "));
    Serial.println(percent);
    // ============ POWER MEASUREMENT ================ //
    percent = double(avgTimePowerMeasurement) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Power Time                  : "));
    Serial.println(avgTimePowerMeasurement);
    Serial.print(F("Power Percent               : "));
    Serial.println(percent);
    // ============ PIR ================ //
    percent = double(avgTimePIR) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("PIR Time                    : "));
    Serial.println(avgTimePIR);
    Serial.print(F("PIR Percent                 : "));
    Serial.println(percent);
    // ============ LED ================ //
    percent = double(avgTimeLED) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("LED Time                    : "));
    Serial.println(avgTimeLED);
    Serial.print(F("LED Percent                 : "));
    Serial.println(percent);
    // ============ INFORMATION ================ //
    percent = double(avgTimeInformation) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Info Time                   : "));
    Serial.println(avgTimeInformation);
    Serial.print(F("Info Percent                : "));
    Serial.println(percent);
    // ============ HELPER ================ //
    percent = double(avgTimeHelper) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Helper Time                 : "));
    Serial.println(avgTimeHelper);
    Serial.print(F("Helper Percent              : "));
    Serial.println(percent);
    // ============ FILESYSTEM ================ //
    percent = double(avgTimeFilesystem) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Filesystem Time             : "));
    Serial.println(avgTimeFilesystem);
    Serial.print(F("Filesystem Percent          : "));
    Serial.println(percent);
    // ============ FILESYSTEM ================ //
    percent = double(avgTimeParameterhandler) / double(avgTimeAll) * 100;
    helper.InsertPrint();
    Serial.print(F("Parameterhandler Time       : "));
    Serial.println(avgTimeParameterhandler);
    Serial.print(F("Parameterhandler Percent    : "));
    Serial.println(percent);
    // ==== All
    helper.InsertPrint();
    Serial.print(F("All Time        : "));
    Serial.println(avgTimeAll);

    helper.BottomSpacerPrint();
}