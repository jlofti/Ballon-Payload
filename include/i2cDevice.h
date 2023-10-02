#include <stdio.h>
#include "hardware/i2c.h"

#ifndef I2C_DEVICE
#define I2C_DEVICE
class i2cDevice
{
public:
    int regWrite(i2c_inst_t *i2c,
                 const uint8_t addr,
                 const uint8_t reg,
                 uint8_t *buf,
                 const uint8_t nbytes);
    int regRead(i2c_inst_t *i2c,
                const uint8_t addr,
                const uint8_t reg,
                uint8_t *buf,
                const uint8_t nbytes);
};
#endif