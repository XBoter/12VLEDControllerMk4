#include <LEDControllerMk4.h>

/*
  Before uploading make sure that the following settings are the same:
  - Flash Size: "4MB (FS:1MB OTA:~1019KB)"

  We only need 1MB for the file system (o be honest, a lot less, but there is no other setting)
  For the OTA update we take the max amount of 1019KB the settings give us
  This settings prevents that when uploading the Sketch via OTA the file system config gets removed
  Tested at 10.02.2021
*/

LEDControllerMk4 controller;

void setup()
{
  controller._setup();
}

void loop()
{
  controller._loop();
}
