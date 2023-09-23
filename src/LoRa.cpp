#include "../include/LoRa.h"

LoRa_t::LoRa_t()
{
    // Default freq 433Mhz
    LoRa_t(433E6);
};

LoRa_t::LoRa_t(long freq_)
{
    begin(freq_);
};

bool LoRa_t::begin(long freq_)
{
    radio = *new LoRaClass;

    if (radio.begin(freq_))

    {
        connected = true;
        return true;
    }

    else
    {
        return false;
    }
}

void LoRa_t::send(string msg_)
{
    printf("Transmitting %s\n", msg_.c_str());
    radio.beginPacket();
    radio.print(msg_.c_str());
    radio.endPacket();
    msgCount++;
}