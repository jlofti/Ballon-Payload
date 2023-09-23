#include "../include/Pressure.h"

PressureSensor_t::PressureSensor_t(){};
PressureSensor_t::PressureSensor_t(i2c_inst_t *i2c_)
{
    begin(i2c_);
}

bool PressureSensor_t::begin(i2c_inst_t *i2c_)
{
    i2c = i2c_;

    printf("Searching for pressure sensor...\n");
    uint8_t rData = 0;
    for (int i = 0; i < 10; i++)
    {
        // Will be not -1 if fail
        uint8_t read = i2c_read_blocking(i2c, PRESSURE_ADDR, &rData, 1, false);
        if (read != -1)
        {
            printf("Pressure Sensor found\n");

            // Perform reset and PROM read
            reset();
            readPROM();
            connected = true;
            return true;
        }

        printf("...");
        sleep_ms(10);
    }

    printf("Pressure Sensor not found\n");
    return false;
}

uint8_t PressureSensor_t::reset()
{
    // Reset command before run
    return i2c_write_blocking(i2c, PRESSURE_ADDR, &PRESSURE_RESET, 1, false);
}
uint8_t PressureSensor_t::readPROM()
{
    // Start at this command and increase by 2 to read each PROM value
    uint8_t PROM_CMD = 0xA0;
    uint8_t bytesRead = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t buffer[2];
        buffer[0] = 0;
        buffer[1] = 0;
        i2c_write_blocking(i2c, PRESSURE_ADDR, &PROM_CMD, 1, false);
        sleep_ms(100);
        bytesRead += i2c_read_blocking(i2c, PRESSURE_ADDR, &buffer[0], 1, false);
        bytesRead += i2c_read_blocking(i2c, PRESSURE_ADDR, &buffer[1], 1, false);

        PROMBuff[i] = (buffer[0] << 8) | buffer[1];
        PROM_CMD += 2;
    }

    return bytesRead;
}

uint8_t PressureSensor_t::readTempPressure()

{
    uint8_t written = 0;

    // Set coeffs
    uint16_t C1 = PROMBuff[0];
    uint16_t C2 = PROMBuff[1];
    uint16_t C3 = PROMBuff[2];
    uint16_t C4 = PROMBuff[3];
    uint16_t C5 = PROMBuff[4];
    uint16_t C6 = PROMBuff[5];

    // Begin Pressure Conversion
    written += i2c_write_blocking(i2c, PRESSURE_ADDR, &CONVERT_PRESSURE, 1, false);
    sleep_ms(100);
    written += i2c_write_blocking(i2c, PRESSURE_ADDR, &ADC_READ, 1, false);
    sleep_ms(100);

    // printf("Pressure Bytes Written: %d\n", written);

    uint8_t pressureBuff[3];
    uint8_t tempBuff[3];

    // Read Pressure
    i2c_read_blocking(i2c, PRESSURE_ADDR, pressureBuff, 3, false);

    // Begin Temp Conversion
    written += i2c_write_blocking(i2c, PRESSURE_ADDR, &CONVERT_TEMP, 1, false);
    sleep_ms(100);
    written += i2c_write_blocking(i2c, PRESSURE_ADDR, &ADC_READ, 1, false);
    sleep_ms(100);

    // printf("Temp Bytes Written: %d\n", written);

    i2c_read_blocking(i2c, PRESSURE_ADDR, tempBuff, 3, false);

    // Math
    unsigned int D1 = ((pressureBuff[0] << 16) | (pressureBuff[1] << 8) | (pressureBuff[2]));
    unsigned int D2 = ((tempBuff[0] << 16) | (tempBuff[1] << 8) | (tempBuff[2]));

    // printf("Read D1: %d\n", D1);
    // printf("Read D2: %d\n", D2);

    // Math
    signed int dT = D2 - C5 * (1 << 8);
    signed int TEMP = 2000 + dT * C6 / (1 << 23);

    // printf("Read dT: %d\n", dT);

    // Math
    long OFF = C2 * (1 << 17) + (C4 * dT) / (1 << 6);
    long SENS = C1 * (1 << 16) + (C3 * dT) / (1 << 7);

    // printf("Read OFF: %d\n", OFF);
    // printf("Read SENS: %d\n", SENS);

    // Math
    signed int P = (D1 * SENS / (1 << 21) - OFF) / (1 << 15);

    // printf("Temp: %d\n", TEMP);
    // printf("Pressure %d\n", P);

    temp = TEMP;
    pressure = P;

    return written;
}

void PressureSensor_t::printTemp()
{
    // Prin the temperature in celcius
    printf("Sensor temp is: %5.2f C\n", (float)temp / 100);
}

void PressureSensor_t::printPressure()
{
    // Print the pressure in mbar
    printf("Sensor pressure is: %5.2f mBar\n", (float)pressure / 100);
}

void PressureSensor_t::printPROM()
{
    // Print each PROM value
    for (int i = 0; i < 8; i++)
    {
        printf("PROM Value: %d\n", PROMBuff[i]);
    }
}