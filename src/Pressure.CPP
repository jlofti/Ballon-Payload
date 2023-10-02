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

            connected = true;
            return true;
        }

        printf("...");
        sleep_ms(10);
    }

    printf("Pressure Sensor not found\n");
    return false;
}

float PressureSensor_t::readPressure()
{
    float p;
    uint8_t presH;
    uint8_t presL;
    uint8_t presXL;

    // Start one shot and wait till ready
    this->startOneShot();

    // Read 3 pressure regs
    this->regRead(i2c, PRESSURE_ADDR, PRES_XL_REG, &presXL, 1);
    this->regRead(i2c, PRESSURE_ADDR, PRES_L_REG, &presL, 1);
    this->regRead(i2c, PRESSURE_ADDR, PRES_H_REG, &presH, 1);

    // Convert
    p = ((static_cast<uint32_t>(presH << 16)) | static_cast<uint32_t>(presL << 8) | presXL);
    p = (p / 4096.0);

    return p;
}

void PressureSensor_t::startOneShot()
{
    // Initiate one shot of pressure

    this->regWrite(i2c, PRESSURE_ADDR, CTRL2_REG, &ONE_SHOT, 1);
    uint8_t oneShotStatus;

    // Wait till bit 0 in ctrl2 register clears
    do
    {
        this->regRead(i2c, PRESSURE_ADDR, CTRL2_REG, &oneShotStatus, 1);
        oneShotStatus = oneShotStatus & 0x01;
    } while (oneShotStatus != 0);
}

float PressureSensor_t::readTemp()
{
    uint8_t tempH;
    uint8_t tempL;
    float temp;

    // Initiate temp measurement
    this->startOneShot();

    // Read two temp regs
    this->regRead(i2c, PRESSURE_ADDR, TEMP_L_REG, &tempL, 1);
    this->regRead(i2c, PRESSURE_ADDR, TEMP_H_REG, &tempH, 1);

    // Convert
    temp = (static_cast<uint16_t>(tempH << 8)) | tempL;
    temp = temp / 100.0;
    return temp;
}

float PressureSensor_t::readAlt()
{
    float alt = 44330.0 * (1.0 - pow(this->pressure / PRES_SEA_HPA, (1.0 / 5.255)));
    return alt;
}

void PressureSensor_t::setPressure(float p)
{
    this->pressure = p;
}

void PressureSensor_t::setTemp(float t)
{
    this->temp = t;
}

void PressureSensor_t::setAlt(float a)
{
    this->alt = a;
}

void PressureSensor_t::printPressure()
{
    printf("Pressure Sensor Pressure: %f mBar\n", this->pressure);
}

void PressureSensor_t::printTemp()
{
    printf("Pressure Sensor temp: %f celcius\n", this->temp);
}

void PressureSensor_t::printAlt()
{
    printf("Pressure Sensor altitude: %f meters\n", this->alt);
}