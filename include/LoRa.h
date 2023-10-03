#include "./pico-lora/src/LoRa-RP2040.h"

class LoRa_t
{
public:
    LoRaClass radio;
    bool connected = false;
    int msgCount = 0;

    LoRa_t();
    LoRa_t(long freq_);

    /*
     * Sets up radio module and verifies operability
     *
     * @param freq_ frequency to run attention
     * @return True if succesful, false otherwise
     */
    bool begin(long freq_);

    /*
     * Transmits String and increases msgCount
     *
     * @param msg_ string to be transmitted
     */
    void send(string msg_);
};