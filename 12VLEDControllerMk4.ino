#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "I2C.h"
#include "PowerMeasurement.h"
#include "LedDriver.h"
#include "Network.h"

using namespace LedControllerSoftwareMk5;

//-------------------- Basic Information --------------------//
#define Name "12V LED Controller Mk4"
#define Programmer "Nico Weidenfeller"
#define Created "28.06.2020"
#define LastModifed "30.06.2020"
#define Version "0.0.1"
/*
   ToDo           :  -

   Bugs           :  -

   Patchhistory   :  - Version 0.0.1
                         Init Commit

*/

//++++ Global Defines ++++//
#define BAUDRATE 115200
#define PCA9685PW_I2C_ADDRESS 0x40
#define INA219AIDR_I2C_ADDRESS 0x45

// I2C communication
I2C *i2c = new I2C();
Network *network = new Network();
PowerMeasurement *powerMessurement = new PowerMeasurement(INA219AIDR_I2C_ADDRESS, i2c, network);
LedDriver *ledDriver = new LedDriver(PCA9685PW_I2C_ADDRESS, i2c, network);

void setup()
{
  // Setup Serial
  Serial.begin(BAUDRATE);
  delay(100);

  // Info and Version print
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
  Serial.println("");

  i2c->Init();
  powerMessurement->Init();
  ledDriver->Init();
  network->Init();
}

void loop()
{
  network->Run();
  ledDriver->Run();
  powerMessurement->Run();
}
