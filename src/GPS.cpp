#include "../include/GPS.h"

GPS_t::GPS_t(){};
GPS_t::GPS_t(i2c_inst_t *i2c_)
{
    begin(i2c_);
};

bool GPS_t::begin(i2c_inst_t *i2c_)
{
    i2c = i2c_;

    printf("Searching for GPS...\n");
    uint8_t rAddr;

    // Will return -1 if not acknowledged
    for (int i = 0; i < 10; i++)
    {
        uint8_t read = i2c_read_blocking(i2c, GPS_ADDR, &rAddr, 1, false);
        if (read != -1)
        {
            printf("GPS discovered\n");
            connected = true;
            return true;
        }

        printf("...");
        sleep_ms(100);
    }

    printf("GPS not found\n");
    return false;
}

uint16_t GPS_t::getAvailableReadBytes()
{

    // Write to MSB Reg to get available bytes
    int written = i2c_write_blocking(i2c, GPS_ADDR, &MSB_AVAILABLE_REG, 1, true);

    if (written == 1)
    {
        uint8_t msb;
        uint8_t lsb;
        uint16_t availableBytes;
        i2c_read_blocking(i2c, GPS_ADDR, &msb, 1, true);
        i2c_read_blocking(i2c, GPS_ADDR, &lsb, 1, false);

        availableBytes = (msb << 8) | lsb;
        return availableBytes;
    }

    else
    {
        return 0;
    }
}

uint16_t GPS_t::readI2CGPS(uint16_t availableBytes)
{

    // Nothing to read, dont do anything
    if (availableBytes == 0)
    {
        return 0;
    }

    else
    {
        this->data = "";
        uint16_t bytesRead = 0;
        bool nostop = true;
        uint8_t c;

        // Read bytes
        for (int i = 0; i < availableBytes; i++)
        {

            if (i == availableBytes - 1)
            {
                nostop = false;
            }

            bytesRead += i2c_read_blocking(i2c, GPS_ADDR, &c, 1, nostop);
            this->data.push_back(c);
        }
        return bytesRead;
    }
}

void GPS_t::printBuff()

{

    // If empty data, dont print anything
    if (this->data.compare("") == 0)
    {
        return;
    }

    // Extrat the GNGGA sentence
    std::string begin = "$GNGGA";
    std::string end = "\r\n";

    // std::string buff = (char *)dataBuff;

    int beginIndex = this->data.find(begin);

    int endIndex = -1;
    int pos = 0;

    while (endIndex < beginIndex)
    {
        endIndex = this->data.find(end, pos);
        pos += 1;
    }

    std::string msg = this->data.substr(beginIndex, endIndex - beginIndex);

    // Print the GNGGA sentence
    printf("%s\n", msg.c_str());
}