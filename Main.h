// Include guard
#ifndef MAIN_H_INCLUDE
#define MAIN_H_INCLUDE

#include "I2C.h"
#include "PowerMeasurement.h"
#include "LedDriver.h"
#include "Network.h"
#include "Information.h"

//-------------------- Basic Information --------------------//
#define Name        "12V LED Controller Mk4"
#define Programmer  "Nico Weidenfeller"
#define Created     "28.06.2020"
#define LastModifed "25.07.2020"
#define Version     "0.4.0"

/*

   ToDo           :  - Fix bug with power measurements reading zero from reg

   Bugs           :  -

   Patchhistory   :  - Version 0.0.1
                            Init Commit
                     - Version 0.1.0
                            Basic functions implemented
                     - Version 0.2.0
                            Bug fixes and added info print
                     - Version 0.3.0
                            Added mqtt electrical measurements publish and heartbeat mqtt publish.
                     - Version 0.4.0
                            Added functions comments to the i2c functions and added error info for write8 and write16
*/

//++++ Global Defines ++++//

#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45 


class Main
{
    // ## Data ## //
    private:

    public:
       LedControllerSoftwareMk5::I2C i2c = LedControllerSoftwareMk5::I2C();
       LedControllerSoftwareMk5::Network network = LedControllerSoftwareMk5::Network();
       LedControllerSoftwareMk5::PowerMeasurement powerMessurement = LedControllerSoftwareMk5::PowerMeasurement(INA219AIDR_I2C_ADDRESS, &i2c, &network);
       LedControllerSoftwareMk5::LedDriver ledDriver = LedControllerSoftwareMk5::LedDriver(INA219AIDR_I2C_ADDRESS, &i2c, &network);
       LedControllerSoftwareMk5::Information information = LedControllerSoftwareMk5::Information(&network);

    // ## Functions ## //
    private:

    public:
       Main();
       void _loop();
       void _setup();
};

extern Main mainController;


#endif