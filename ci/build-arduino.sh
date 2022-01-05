#!/bin/bash
# Exit immediately if a command exits with a non-zero status.
set -e
# Enable the globstar shell option
shopt -s globstar
# Make sure we are inside the github workspace
cd $GITHUB_WORKSPACE
# Create directories
mkdir $HOME/Arduino
mkdir $HOME/Arduino/libraries
# Install Arduino IDE
export PATH=$PATH:$GITHUB_WORKSPACE/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
# Create Config
arduino-cli config init
# Add ESP8266 board to config
arduino-cli config add board_manager.additional_urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
# Install ESP8266 core
arduino-cli core install esp8266:esp8266
# List Installed Boards
arduino-cli core list
# Install NTPClient Library
arduino-cli lib install NTPClient
# Install PubSubClient Library
arduino-cli lib install pubsubclient
# Install ArduinoJson Library 
arduino-cli lib install ArduinoJson
# Link Arduino library
ln -s $GITHUB_WORKSPACE $HOME/Arduino/libraries/CI_Test_Library
# Compile all *.ino files for the Arduino Uno
for f in **/*.ino ; do
    arduino-cli compile --fqbn esp8266:esp8266:nodemcu $f
done