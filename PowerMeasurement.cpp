#include "PowerMeasurement.h"

using namespace LedControllerSoftwareMk5;

PowerMeasurement::PowerMeasurement(uint8_t i2cAddress, I2C *i2c, Network *network)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
}

void PowerMeasurement::Init()
{
    if (!init)
    {
        i2c->Init();

        calc();

        // -- Configuring INA219
        // Bit 15 Reset
        // Bit 14 Not used
        // Bit 13 Bus Voltage Range (0 = 16V, 1 = 32V)
        // Bit 11-12 Programmable Gain Amplifier
        // Bit 7-10 Bus ADC
        // Bit 3-6 Shunt ADC
        // Bit 2-0 Mode
        uint16_t config = 0b0001100110011111;
        i2c->write16(i2cAddress, regConfig, config);

        // -- Calibrating INA219
        //uint16_t calibration = (uint16_t)calibrationValue;
        uint16_t calibration = (uint16_t)correctedCalibrationValue;
        i2c->write16(i2cAddress, regCalibration, calibration);

        Serial.println("Power Measurement Unit initialized");
        init = true;
    }
}

void PowerMeasurement::Run()
{
    // Check if WiFi or Mqtt is connected
    if (network->wifiConnected || network->mqttConnected)
    {
        unsigned long CurMillis_PowerMessurmentUpdateRate = millis();
        if (CurMillis_PowerMessurmentUpdateRate - PrevMillis_PowerMessurmentUpdateRate >= TimeOut_PowerMessurmentUpdateRate)
        {
            PrevMillis_PowerMessurmentUpdateRate = CurMillis_PowerMessurmentUpdateRate;

            uint16_t calibration = (uint16_t)calibrationValue;

            uint16_t shuntVoltageRaw = i2c->read16(i2cAddress, regShuntVolt);
            float shuntVoltage = shuntVoltageRaw * 0.01; // mV

            uint16_t _busVoltageRaw = i2c->read16(i2cAddress, regBusVolt);
            int16_t busVoltageRaw = (int16_t)((_busVoltageRaw >> 3) * 4);
            float busVoltage = busVoltageRaw * 0.001; // V

        Serial.println();

            // Sometimes a sharp load will reset the INA219, which will
            // reset the cal register, meaning CURRENT and POWER will
            // not be available ... avoid this by always setting a cal
            // value even if it's an unfortunate extra step
            i2c->write16(i2cAddress, regCalibration, calibration);
            uint16_t currentRaw = i2c->read16(i2cAddress, regCurrent);
            Serial.print("Current Raw : ");
            Serial.println(currentRaw);
            double current_mA = currentRaw * currentLSB;

            // Sometimes a sharp load will reset the INA219, which will
            // reset the cal register, meaning CURRENT and POWER will
            // not be available ... avoid this by always setting a cal
            // value even if it's an unfortunate extra step
            i2c->write16(i2cAddress, regCalibration, calibration);
            uint16_t powerRaw = i2c->read16(i2cAddress, regPower);
           Serial.print("Power Raw : ");
            Serial.println(powerRaw); 
            float power_mW = powerRaw * (currentLSB * 20);

            Serial.println();
            Serial.print("Shunt Voltage : ");
            Serial.print(shuntVoltage);
            Serial.println(" mV");

            Serial.print("Bus Voltage   : ");
            Serial.print(busVoltage);
            Serial.println(" V");

            Serial.print("Power         : ");
            printDouble(power_mW, 10000);
            Serial.println(" mW");

            Serial.print("Current       : ");
            printDouble(current_mA, 10000);
            Serial.println(" mA");
        }
    }
    else
    {
    }
}

void PowerMeasurement::calc()
{
    // VOLTAGE_BUS_MAX = 16V             (Assumes 32V, can also be set to 16V)
    // VOLTAGE_SHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
    // RESISTOR_SHUNT = 0.002             (Resistor value in ohms)

    const double VOLTAGE_BUS_MAX = 16.0;   // Voltage
    const double VOLTAGE_SHUNT_MAX = 0.32; // Voltage
    const double RESISTOR_SHUNT = 0.002;   // Ohm

    double MaxPossibleCurrent = VOLTAGE_BUS_MAX / RESISTOR_SHUNT;
    Serial.print("MaxPossibleCurrent : ");
    printDouble(MaxPossibleCurrent, 10000);
    Serial.println("");

    const double MAX_EXPECTED_CURRENT = 5.0; // Ampere

    currentLSB = MAX_EXPECTED_CURRENT / 32767.0;
    Serial.print("CurrentLSB : ");
    printDouble(currentLSB, 10000);
    Serial.println("");

    calibrationValue = trunc(0.04096 / (currentLSB * RESISTOR_SHUNT));
    Serial.print("Calibration Value : ");
    printDouble(calibrationValue, 10000);
    Serial.println("");

    correctedCalibrationValue = trunc((calibrationValue * 0.0037) / 0.000001);
    Serial.print("Corrected Calibration Value : ");
    printDouble(correctedCalibrationValue, 10000);
    Serial.println("");

}

void PowerMeasurement::printDouble(double val, unsigned int precision)
{
    // prints val with number of decimal places determine by precision
    // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
    // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

    Serial.print(int(val)); //prints the int part
    Serial.print(".");      // print the decimal point
    unsigned int frac;
    if (val >= 0)
        frac = (val - int(val)) * precision;
    else
        frac = (int(val) - val) * precision;
    int frac1 = frac;
    while (frac1 /= 10)
        precision /= 10;
    precision /= 10;
    while (precision /= 10)
        Serial.print("0");

    Serial.print(frac, DEC);
}