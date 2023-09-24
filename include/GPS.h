#include "hardware/i2c.h"
#include <stdio.h>
#include <string>

static const uint8_t MSB_AVAILABLE_REG = 0xFD;
static const uint8_t LSB_AVAILABLE_REG = 0xFE;
static const uint8_t DATA_BUFFER_REG = 0xFF;
#define GPS_ADDR 0x42

class GPS_t
{

public:
    i2c_inst_t *i2c;
    std::string data = "";
    bool connected = false;

    GPS_t();
    GPS_t(i2c_inst_t *i2c_);

    /*
     * Begins and verifies GPS connection on provided i2c instance
     *
     * @param i2c_ pointer to i2c_inst_t for GPS to use
     * @return True if GPS is succesfully discovered on I2C line, false if otherwise
     */
    bool begin(i2c_inst_t *i2c_);

    /*
     * Reads the bytes stored in the addresses 0xFD and 0xFE to see how many bytes are readable from the GPS message buffer stream
     *
     * @return Number of bytes available to read
     */
    uint16_t getAvailableReadBytes();

    /*
     * Reads the GPS message buffer stream at address 0xFF
     *
     * @return Number of bytes successfully read
     */
    uint16_t readI2CGPS(uint16_t availableBytes);

    /*
     * Prints out the GNGGA message in the data buffer
     *
     */
    void printBuff();
};