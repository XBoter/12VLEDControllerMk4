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

            // Get shunt voltage
            uint16_t shuntVoltageRaw = i2c->read16(i2cAddress, regShuntVolt);
            valueShunt_mV = shuntVoltageRaw * 0.01; // mV

            // Get bus voltage
            uint16_t _busVoltageRaw = i2c->read16(i2cAddress, regBusVolt);
            int16_t busVoltageRaw = (int16_t)((_busVoltageRaw >> 3) * 4);
            valueBus_V = busVoltageRaw * 0.001; // V

            /* 
            For calc values look up https://datasheet.lcsc.com/szlcsc/1810181516_Texas-Instruments-INA219AIDR_C138706.pdf
            */
            double cal_Value = 122650.0; // Calibration Value
            double mA_Multiplyer = 0.2;  // 0.2mA/Bit
            double mW_Multiplyer = 4.0;  // 4mW/Bit

            // Calc current
            double current = (((double)shuntVoltageRaw * (double)cal_Value) / 4096.0);
            valueCurrent_mA = current * mA_Multiplyer;

            // Calc power
            double power = (((double)current * (double)busVoltageRaw) / 5000.0);
            valuePower_mW = power * mW_Multiplyer;
        }
    }
    else
    {
        valueShunt_mV = 0.0;
        valueBus_V = 0.0;
        valuePower_mW = 0.0;
        valueCurrent_mA = 0.0;
    }
}
