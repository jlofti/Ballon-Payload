#include "hardware/i2c.h"
#include <stdio.h>
#include <string>
#include "./i2cDevice.h"

static uint8_t MSB_AVAILABLE_REG = 0xFD;
static uint8_t LSB_AVAILABLE_REG = 0xFE;
static uint8_t DATA_BUFFER_REG = 0xFF;
static uint8_t GPS_ADDR = 0x42;
#define GPS_ADDR 0x42

#define IDENTIFIER_FIELD 0
#define TIME_FIELD 1
#define LAT_FIELD 2
#define LAT_NS_FIELD 3
#define LON_FIELD 4
#define LON_EW_FIELD 5
#define QUALITY_FIELD 6
#define SAT_FIELD 7
#define HDOP_FIELD 8
#define ALT_FIELD 9
#define ALT_UNIT_FIELD 10
#define GEOD_FIELD 11
#define GEOD_UNIT_FIELD 12
#define DGPS_FIELD 13
#define CHECKSUM 14

class GPS_t : i2cDevice
{

public:
    i2c_inst_t *i2c;
    std::string data = "";
    bool connected = false;

    float time;
    float lat;
    float lon;
    float alt;

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
    uint16_t readGPS(uint8_t *buf, uint16_t availableBytes);

    void setData(uint8_t *data, uint16_t len);

    bool extractGNGGA(uint8_t *data_, uint16_t len_, std::string *sentence_);

    void setAndExtractGNGGAFields(std::string *sentence_);

    void printLat();
    void printLon();
    void printAlt();
    void printTime();
};