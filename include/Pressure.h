#include <stdio.h>
#include "hardware/i2c.h"

static const uint8_t PRESSURE_ADDR = 0x76;
static const uint8_t PRESSURE_ADDRW = 0xEC;
static const uint8_t PRESSURE_ADDRR = 0xED;
static const uint8_t CONVERT_PRESSURE = 0x40;
static const uint8_t CONVERT_TEMP = 0x50;
static const uint8_t ADC_READ = 0x00;
static const uint8_t PRESSURE_RESET = 0x1E;

static const uint8_t PROM0 = 0xA0;
static const uint8_t PROM1 = 0xA2;
static const uint8_t PROM2 = 0xA4;
static const uint8_t PROM3 = 0xA6;
static const uint8_t PROM4 = 0xA8;
static const uint8_t PROM5 = 0xAA;
static const uint8_t PROM6 = 0xAC;
static const uint8_t PROM7 = 0xAE;

class PressureSensor_t
{
public:
    signed int temp = -1;
    signed int pressure = -1;
    i2c_inst_t *i2c;
    uint16_t PROMBuff[8];
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

    /*
     * Resets pressure sensor before running
     *
     * @return Number of bytes written. Should be 1
     */
    uint8_t reset();

    /*
     * Reads and converts temperature and pressure
     *
     * @return Number of bytes written. Should be 2
     */
    uint8_t readTempPressure();

    /*
     * Reads PROM data required for temperature and pressure calculation
     *
     * @return Number of bytes read
     */
    uint8_t readPROM();

    /*
     * Prints PROM data
     */
    void printPROM();

    /*
     * Prints last calculated temperature
     */
    void printTemp();

    /*
     * Prints last calculated pressure
     */
    void printPressure();
};