#include "../include/i2cDevice.h"

int i2cDevice::regRead(i2c_inst_t *i2c, const uint8_t addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes)
{
    int read = 0;
    i2c_write_blocking(i2c, addr, &reg, 1, true);
    read += i2c_read_blocking(i2c, addr, buf, nbytes, false);
    return read;
};

int i2cDevice::regWrite(i2c_inst_t *i2c, const uint8_t addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes)
{
    int num_bytes_read = 0;
    uint8_t msg[nbytes + 1];

    // Check to make sure caller is sending 1 or more bytes
    if (nbytes < 1)
    {
        return 0;
    }

    // Append register address to front of data packet
    msg[0] = reg;
    for (int i = 0; i < nbytes; i++)
    {
        msg[i + 1] = buf[i];
    }

    // Write data to register(s) over I2C
    i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

    return num_bytes_read;
};