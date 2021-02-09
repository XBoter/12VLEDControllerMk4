#include "../../src/LEDControllerMk4.h"

LEDControllerMk4 controller;

void setup()
{
  controller._setup();
}

void loop()
{
  controller._loop();
}