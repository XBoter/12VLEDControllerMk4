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

        /*
            Calculations INA219
            Datasheet: https://datasheet.lcsc.com/szlcsc/1810181516_Texas-Instruments-INA219AIDR_C138706.pdf

                                         
            LSB Shunt Voltage Register = +-0.04V / 4096 = 0.000009765V == 9.77μV ≈ 10μV <== !VALUE FROM DATASHEET!
            LSB Bus Voltage Register = 16V / 4096 = 0,0039V == 3.90mV ≈ 4mV <== !VALUE FROM DATASHEET!

            VShunt = Shunt Voltage Register * 10μV
            VBus = Bus Voltage Register * 4mV

            VBusMax = 16V

            VShuntMax = +-80mV  (PGA Register Options: +-40mV +-80mV +-160mV +-320mV)
            RShunt = 0.02Ω

            IMaxExpected = 3A
            IMaxPossible = VShuntMax / RShunt => 0.08V / 0.02Ω = 4A 

            LSBMinimum = IMaxExpected / 32767 => 3A / 32767 = 91.55 μA
            LSBMaximum = IMaxExpected / 4096 => 3A / 4096 = 732.42 μA

            LSBMinimum < LSBCurrent < LSBmaximum
            LSBCurrent = 92μA

            LSBPower = 20 * LSBCurrent => 20 * 92μA = 1840μA == 1.84mA

            CalibrationRegister = trunc(0.04096 / (LSBCurrent + RShunt)) => trunc(0.04096 / (0.000092A * 0.02Ω)) = 22261 == 0b101011011110101

            IMaxShuntCalc = CurrentLSB * 32767 = 92μA * 32767 = 3.014564A

            IF IMaxShuntCalc >= IMaxPossbile then: IMaxBeforeOverflow = IMaxPossible
            else: IMaxBeforeOverflow = IMaxShuntCalc

            IF 3.014564A >= then 4A: IMaxBeforeOverflow = 4A
            else: IMaxBeforeOverflow = 3.014564A 

            !!! => IMaxBeforeOverflow = 3.014564A 

            VMaxShuntCalc = IMaxBeforeOverflow * RShunt => 3.014564A * 0.02Ω = 0.060V

            IF VMaxShuntCalc >= VShuntMax then: VMaxShuntBeforeOverflow = VShuntMax
            else: VMaxShuntBeforeOverflow = VMaxShuntCalc

            IF 0.06V >= then 0.08V: VMaxShuntBeforeOverflow = 0.08V
            else: VMaxShuntBeforeOverflow = 0.06V

            !!! => VMaxShuntBeforeOverflow = 0.06V 

            PMax = IMaxBeforeOverflow * VBusMax => 3.014564A * 16V = 48.23W

        */

        // ==== Config Register Structure ==== //
        // Bit 15 Reset         : 0
        // Bit 14 -             : 0
        // Bit 13 BRNG          : 0
        // Bit 12 PG1           : 0
        // Bit 11 PG0           : 1
        // Bit 10 BADC 4        : 0
        // Bit 9  BADC 3        : 0
        // Bit 8  BADC 2        : 1
        // Bit 7  BADC 1        : 1
        // Bit 6  SADC 4        : 0
        // Bit 5  SADC 3        : 0
        // Bit 4  SADC 2        : 1
        // Bit 3  SADC 1        : 1
        // Bit 2  MODE 3        : 1
        // Bit 1  MODE 2        : 1
        // Bit 0  MODE 1        : 1

        uint16_t ConfigRegister = 0b0000100110011111;

        Serial.println("Setting config register");
        i2c->write16(i2cAddress, CONFIG, ConfigRegister);
        Serial.print("Config Register : ");
        Print2ByteValue(i2c->read16(i2cAddress, CONFIG));

        uint16_t CalibrationRegister = 0b101011011110101;

        Serial.println("Setting calibration register");
        i2c->write16(i2cAddress, CALIBRATION, CalibrationRegister);
        Serial.print("Calibration Register : ");
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
    if (!init)
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

            // Get Register Values
            uint16_t ShuntVoltageRegister = i2c->read16(i2cAddress, SHUNT_VOLTAGE);
            uint16_t BusVoltageRegister = i2c->read16(i2cAddress, BUS_VOLTAGE);
            uint16_t BusVoltageRegisterShift = (BusVoltageRegister >> 3) * 4;

            // Calc VShunt and VBus
            double VShunt = ShuntVoltageRegister * 0.01;   // Convert to mV
            double VBus = BusVoltageRegisterShift * 0.001; // Convert to V

            //Serial.print("mVShunt ");
            //Serial.print(VShunt);
            //Serial.print(" | VBus ");
            //Serial.print(VBus);

            // Calc Current
            //double CurrentBitMultiplier = 0.092;  // 0.092mA/Bit
            double CurrentBitMultiplier = 0.015; // 0.015mA/Bit => LSB Calc based on max current of 0.5A

            double current = (double)ShuntVoltageRegister * CurrentBitMultiplier * 10.0; // Value is in mA
            //Serial.print(" | mABus ");
            //Serial.print(current);

            // Calc Power
            double power = current * (VBus / 1000);
            //Serial.print(" | WPower ");
            //Serial.print(power);

            //Serial.println("");
        }
        else
        {
            valueShunt_mV = 0.0;
            valueBus_V = 0.0;
            valuePower_mW = 0.0;
            valueCurrent_mA = 0.0;
        }

        // Publish measured data
        this->network->PublishElectricalMeasurement();
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