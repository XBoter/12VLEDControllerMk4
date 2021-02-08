# 12VLEDControllerMk4

## Features
- Control for 2x 12V RGBCWWW LED Strips 
- Support for 2x 5V PIR Motion Sensors
- NTP time based brightness adjustments for motion detection 
- Phase Shifted PWM channels for load balancing while dimming
- Current and voltage measurements
- Flash storage of configuration against power lose
- Webpage based configuration via accesspoint
- Controllabled via Homeassistant and JSON
- Programmed in C++ and Arduino via VS Code
- Nodemcu ESP8266 microcontroller with WiFi
- OTA Sketch updates possible


## Roadmap 
- OTA Github updates of Sketch
- Rework of power messurement unit
- Sync option for led strip effects on both strips
- STL for MK2 PIR Housing
- Wiring diagram


## Known Bug's
- PCA9685 thrwos cassualy IC2 errors => needs reset
- Power messurment unit calc wrong => fixing calc with new shunt resistor

## Wiki
For more information and guids for installation and configuration head over to the [Wiki](https://github.com/XBoter/12VLEDControllerMk4/wiki)

## Warning
I am not responsible for any failure or fire, destruction or other damage caused by the LED controller.
Use at your own risk

## Buy me a coffee or sponsor the project
[![PayPal](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=5UD82M4V6M2XC)  
