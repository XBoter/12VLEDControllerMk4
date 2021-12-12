
# Change Log
All notable changes to this project will be documented in this file.

## Version 1.2.x
- Release date: xx.xx.xxxx


## Version 1.2.8
- Release date: 12.12.2021

### Changed
- Changed max Connection of Configuration Access Point to 1
- The ESP has now the IP 192.168.1.1 when in Access Point in Configure Mode

### Fixed
- Access Point not giving IP to connected client resulted in client disconnecting Access Point


## Version 1.2.7
- Release date: 26.05.2021

### Changed
 - Added a timer for Fade To Black when a short WiFi or MQTT DC happens
 - Enabled republish of Controller Values and change publish frequency to 1 Minute


## Version 1.2.6
- Release date: 28.04.2021

### Changed
- WiFi default hostname to MQTT Client name

### Added
- Multi LED Strip Effect Alarm
- Detailed information print for Network WiFi and MQTT
- Publish of controller Version to MQTT path

### Fixed
- Bug with OTA update overriding file system content
- Bug with Mqtt timeout (Added multiple yield calls for TCP/IP stack)


## Version 1.2.5
- Release date: 10.02.2021

### Changed
- Restructured code base
- Moved Readme instructions to wiki

### Added
- library.json
- library.properties
- Lib examples
- Github Wiki
- Github Bug Report and Feature Request
- Github Project
- Github Workflow for compiling project

### Removed
- Not used code 


## Version 1.2.4 
- Release date: 05.02.2021

### Changed
- No reset of config when not finishing configuration

### Added
- CHANGELOG.md
- Turning off all LEDs when in configure mode

### Fixed
- Bug with interface and initialization of components
 

## Version 1.2.3

- Release date: 05.02.2021

### Changed
- Moved static functions to class
- Reworked reference and initialization of other components

### Fixed
- Minor bug fixes


## Versions < 1.2.3

- Release date: ?

### Changed
- ?

### Added
- ?

### Fixed
- ?