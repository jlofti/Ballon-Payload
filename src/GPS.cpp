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
        uint8_t read = i2c_read_blocking(i2c, GPSAddr, &rAddr, 1, false);
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

uint8_t GPS_t::readI2CBytes()
{

    // Request available read bytes
    uint8_t written = i2c_write_blocking(i2c, GPSAddr, &highAddr, 1, true);

    sleep_ms(1000);

    // Dont read if not available
    uint8_t readAvailable = i2c_get_read_available(i2c);

    // Read the two registers to get available bytes
    if (1)
    {
        uint8_t readBuff[2];
        i2c_read_blocking(i2c, GPSAddr, readBuff, 2, false);
        nBytes = readBuff[0] << 8 | readBuff[1];
        return written;
    }

    else
    {
        nBytes = 0;
        return 0;
    }
}

uint16_t GPS_t::readI2CGPS()
{

    // Get amount of available bytes
    readI2CBytes();

    // Dont read if none available
    if (nBytes == 0)
    {
        return 0;
    }

    // Too many bytes for buffer, flush it
    else if (nBytes > 600)
    {
        uint8_t rData;
        for (int i = 0; i < nBytes; i++)
        {
            i2c_read_blocking(i2c, GPSAddr, &rData, 1, false);
        }
        nBytes = 0;
        return 0;
    }

    // Read bytes
    else
    {
        uint16_t nBytesRead = i2c_read_blocking(i2c, GPSAddr, dataBuff, nBytes, false);
        return nBytesRead;
    }
}

void GPS_t::printBuff()

{

    // If no bytes available, dont print anything
    if (nBytes == 0)
    {
        return;
    }

    // Extrat the GNGGA sentence
    std::string begin = "$GNGGA";
    std::string end = "\r\n";

    std::string buff = (char *)dataBuff;

    int beginIndex = buff.find(begin);

    int endIndex = -1;
    int pos = 0;

    while (endIndex < beginIndex)
    {
        endIndex = buff.find(end, pos);
        pos += 1;
    }

    std::string msg = buff.substr(beginIndex, endIndex - beginIndex);

    // Print the GNGGA sentence
    printf("%s\n", msg.c_str());

    /* if (nBytes != 0)
     {

         for (int i = 0; i < nBytes; i++)
         {
             printf("%c", dataBuff[i]);
         }
         printf("\n");
     }*/
}