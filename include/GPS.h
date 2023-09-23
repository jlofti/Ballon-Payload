#include "hardware/i2c.h"
#include <stdio.h>
#include <string>

static const uint8_t highAddr = 0xFD;
static const uint8_t lowAddr = 0xFE;
static const uint8_t bufferAddr = 0xFF;
static const uint8_t GPSAddr = 0x42;

class GPS_t
{

public:
    i2c_inst_t *i2c;
    uint8_t dataBuff[600];
    uint16_t nBytes = 0;
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
     * @return Number of bytes successfully written
     */
    uint8_t readI2CBytes();

    /*
     * Reads the GPS message buffer stream at address 0xFF
     *
     * @return Number of bytes successfully read
     */
    uint16_t readI2CGPS();

    /*
     * Prints out the GNGGA message in the data buffer
     *
     */
    void printBuff();
};