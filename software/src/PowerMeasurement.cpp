#include "../include/PowerMeasurement.h"


/**
 * Constructor for the PowerMeasurement class
 * 
 * @parameter i2cAddress    The i2c address of the power messurement ic
 * @parameter *i2c          Pointer to the used instance of the I2C class
 * @parameter *network      Pointer to the used instace of the Network class
 */
PowerMeasurement::PowerMeasurement(uint8_t i2cAddress, 
                                   I2C *i2c, 
                                   Network *network,
                                   double shuntResistorOhm)
{
    this->i2cAddress = i2cAddress;
    this->i2c = i2c;
    this->network = network;
    this->shuntResistorOhm = shuntResistorOhm;
}


/**
 * Does init stuff for the PowerMeasurement component
 * 
 * @return True if successfull, false if not 
 */
bool PowerMeasurement::Init()
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

        // -- Settings
        // Bit 11-12 Programmable Gain Amplifier => 11 => +-320mV
        // Bit 7-10 Bus ADC => 0011 => 12 Bit Resolution => Conversion Time 532 μs
        // Bit 3-6 Shunt ADC => 0011 => 12 Bit Resolution => Conversion Time 532 μs
        // Bit 2-0 Mode => 111 => Shunt and Bus continuous
        
        Serial.println("Setting config register");
        uint16_t config = 0b0001100110011111;
        i2c->write16(i2cAddress, CONFIG, config);
        Serial.print("Register Value    : ");
        Print2ByteValue(i2c->read16(i2cAddress, CONFIG));

        Serial.println("Calculating calibration register value");

        // Calc current LSB
        currentLSB = maxExpectedCurrentA / 32768.0;
        Serial.print("Current LSB       : ");
        Serial.println(currentLSB);

        // Calc power LSB
        powerLSB = 20 * currentLSB;
        Serial.print("Power LSB         : ");
        Serial.println(powerLSB);

        // Calc calibration value
        calibrationValue = (int)(0.04096 / (currentLSB * shuntResistorOhm));
        Serial.print("Calibration Value : ");
        Serial.println(calibrationValue);

        // Bound check calibration value
        if(calibrationValue > 65,536)
        {
            calibrationValue = 0;
            Serial.println("");
            Serial.println("Calibration Value out of range of 16 Bit register");
            Serial.println("Resetting calibration value to 0");
        }

        // Setting calibration value
        Serial.println("Setting calibration register");
        uint16_t calibration = (uint16_t)calibrationValue;
        i2c->write16(i2cAddress, CALIBRATION, calibration);
        Serial.print("Register Value    : ");
        Print2ByteValue(i2c->read16(i2cAddress, CALIBRATION));

        Serial.println("Power Measurement Unit initialized");
        init = true;
    }

    return init;
};


/**
 * Runs the PowerMeasurement component. 
 */
void PowerMeasurement::Run()
{
    if(!init)
    {
        return;
    }
    
    unsigned long CurMillis_PowerMessurmentUpdateRate = millis();
    if (CurMillis_PowerMessurmentUpdateRate - PrevMillis_PowerMessurmentUpdateRate >= TimeOut_PowerMessurmentUpdateRate)
    {
        PrevMillis_PowerMessurmentUpdateRate = CurMillis_PowerMessurmentUpdateRate;

        // Check if WiFi or Mqtt is connected
        if (network->wifiConnected || network->mqttConnected)
        {

            // Get/Calc Shunt Voltage
            uint16_t shuntVoltRegister = i2c->read16(i2cAddress, SHUNT_VOLTAGE);
            valueShunt_mV = shuntVoltRegister * 0.01; // mV

            // Get/Calc BusVoltage
            uint16_t busVoltRegister = i2c->read16(i2cAddress, BUS_VOLTAGE);
            uint16_t busVoltageRaw = (busVoltRegister >> 3) * 4;
            valueBus_V = busVoltageRaw * 0.001; // V

            /* 
            For calc values look up https://datasheet.lcsc.com/szlcsc/1810181516_Texas-Instruments-INA219AIDR_C138706.pdf
            */
            double cal_Value = 122650.0; // Calibration Value
            double mA_Multiplyer = 0.2;  // 0.2mA/Bit
            double mW_Multiplyer = 4.0;  // 4mW/Bit

            // Calc current
            double current = (((double)shuntVoltRegister * (double)cal_Value) / 4096.0);
            valueCurrent_mA = current * mA_Multiplyer;

            // Calc power
            double power = (((double)current * (double)busVoltageRaw) / 5000.0);
            valuePower_mW = power * mW_Multiplyer;

            //PrintAllRegister();

            //Serial.print("Bus Current : ");
            //Serial.println(valueCurrent_mA);
            //Serial.print("Bus Voltage : ");
            //Serial.println(valueBus_V);
            //Serial.print("Power : ");
            //Serial.println(valuePower_mW);
        }
        else
        {
            valueShunt_mV = 0.0;
            valueBus_V = 0.0;
            valuePower_mW = 0.0;
            valueCurrent_mA = 0.0;
        }

        // Publish update to mqtt
        this->network->ElectricalMeasurementUpdate( valuePower_mW,
                                                    valueBus_V,
                                                    valueCurrent_mA);
    }
}


/**
 * Prints all used register values from the INA219
 **/
void PowerMeasurement::PrintAllRegister()
{
    Serial.println("# ==== INA219 REGISTERS ==== #");
    for (uint i = 0; i < 6; i++)
    {
        if (i < 16)
        {
          Serial.print("0");
        }
        uint16_t reg_data = i2c->read16(i2cAddress, i);
        Serial.print(i, HEX);
        Serial.print(" ");
        Print2ByteValue(reg_data);
    }
    Serial.println("# ========================== #");
    Serial.println("");
};


/**
 * Prints a 2 byte value with leading zeros 
 * 
 * @parameter byte  The 2 byte value to print in binary
 **/
void PowerMeasurement::Print2ByteValue(uint16_t data)
{
    // Print high byte
    for (int i = 7; i >= 0; i--)
    {
      Serial.print(bitRead(highByte(data), i));
    }
    // Print low byte
    for (int i = 7; i >= 0; i--)
    {
      Serial.print(bitRead(lowByte(data), i));
    }
    Serial.println("");
};