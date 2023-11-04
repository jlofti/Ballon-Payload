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
    uint8_t msb;
    uint8_t lsb;

    // Read two regs to get available bytes at GPS buffer register
    this->regRead(i2c, GPS_ADDR, MSB_AVAILABLE_REG, &msb, 1);
    this->regRead(i2c, GPS_ADDR, LSB_AVAILABLE_REG, &lsb, 1);

    uint16_t availableReadBytes = (msb << 8) | lsb;

    return availableReadBytes;
}

uint16_t GPS_t::readGPS(uint8_t *buf, uint16_t availableBytes)
{

    // Nothing to read, dont do anything
    if (availableBytes == 0)
    {
        return 0;
    }

    uint16_t bytesRead = 0;
    bytesRead += i2c_read_blocking(i2c, GPS_ADDR, buf, availableBytes, false);

    return bytesRead;
}

void GPS_t::setData(uint8_t *data, uint16_t len)
{
    std::string sentence = "";

    if (this->extractGNGGA(data, len, &sentence))
    {
        this->setAndExtractGNGGAFields(&sentence);
    }
}

bool GPS_t::extractGNGGA(uint8_t *data_, uint16_t len_, std::string *sentence_)
{
    // Nothing actually got read
    if (len_ == 0)
    {
        return 0;
    }

    // Convert to string for easy manipulation
    std::string converted = "";
    for (int i = 0; i < len_; i++)
    {
        converted.push_back(static_cast<char>(data_[i]));
    }

    // Extract gngga sentence
    std::string begin = "$GNGGA";
    std::string end = "\r\n";
    int beginIndex = converted.find(begin);

    // If theres no GNGGA, don't do anything
    if (beginIndex == std::string::npos)
    {
        return 0;
    }

    int endIndex = -1;
    int pos = 0;

    // Find the end of the gngga line
    while (endIndex < beginIndex)
    {
        endIndex = converted.find(end, pos);
        pos += 1;

        if (endIndex == std::string::npos)
        {
            return 0;
        }
    }

    // Extract
    converted = converted.substr(beginIndex, endIndex - beginIndex);

    sentence_->append(converted);
    return 1;
}

void GPS_t::setAndExtractGNGGAFields(std::string *sentence_)
{

    // Fields are between each ,
    size_t pos = 0;
    uint8_t count = 0;
    std::string delim = ",";
    std::string fields[15];

    while ((pos = sentence_->find(delim)) != std::string::npos)
    {
        std::string field = sentence_->substr(0, pos);
        fields[count] = field;
        sentence_->erase(0, pos + delim.length());
        count++;
    }

    // Set all fields
    try
    {
        this->time = std::stof(fields[TIME_FIELD]);
        this->lat = std::stof(fields[LAT_FIELD]) / 100.0;

        if (fields[LAT_NS_FIELD].compare("S") == 0)
        {
            this->lat *= -1;
        }

        this->lon = std::stof(fields[LON_FIELD]) / 100.0;

        if (fields[LON_EW_FIELD].compare("W") == 0)
        {
            this->lon *= -1;
        }

        this->alt = std::stof(fields[ALT_FIELD]);
    }
    catch (...)
    {
        printf("No lock\n");
    };

    return;
}
void GPS_t::printLat()
{
    printf("Latitude is %f\n", this->lat);
};
void GPS_t::printLon()
{
    printf("Longitude is %f\n", this->lon);
};
void GPS_t::printAlt()
{
    printf("Altitude is %f\n", this->alt);
};
void GPS_t::printTime()
{
    printf("Time is %f\n", this->time);
};