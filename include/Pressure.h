#include <stdio.h>
#include "hardware/i2c.h"
#include "./i2cDevice.h"
#include <math.h>

static uint8_t PRESSURE_ADDR = 0x5D;
static uint8_t STATUS_REG = 0x27;
static uint8_t CTRL1_REG = 0x10;
static uint8_t CTRL2_REG = 0x11;

static uint8_t ONE_SHOT = 0x01;

static uint8_t PRES_H_REG = 0x2a;
static uint8_t PRES_L_REG = 0x29;
static uint8_t PRES_XL_REG = 0x28;

static uint8_t TEMP_L_REG = 0x2b;
static uint8_t TEMP_H_REG = 0x2c;

#define PRES_SEA_HPA (1013.25f)

class PressureSensor_t : i2cDevice
{
public:
    float temp;
    float pressure;
    float alt;
    i2c_inst_t *i2c;
    bool connected = false;

    PressureSensor_t();
    PressureSensor_t(i2c_inst_t *i2c_);

    /*
     * Begins and verifies Pressure Sensor connection on provided i2c instance
     *
     * @param i2c_ pointer to i2c_inst_t for Pressure Sensor to use
     * @return True if GPS is succesfully discovered on I2C line, reset, and PROM read. false if otherwise
     */
    bool begin(i2c_inst_t *i2c_);

    float readPressure();
    float readTemp();
    float readAlt();

    void setPressure(float p);
    void setTemp(float t);
    void setAlt(float a);

    void printPressure();
    void printTemp();
    void printAlt();

    void startOneShot();
};