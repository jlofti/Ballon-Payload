#include "pico/sync.h"
#include <memory>
#include <cstring>
#include <string>
#include <stdio.h>
#include "hardware/gpio.h"
#include "./LimitedQueue.h"

#define SID_BITMASK 0x0000FF00
#define PARAM_BITMASK 0x000000FF
#define REGULAR_TRANSMISSION 0xFFFF0400

#define CALLSIGN "W5UL-FB"
#define PADDING "00"

#define CALLSIGN_LENGTH 7
#define SID_LENGTH 1
#define PARAM_LENGTH 1

enum sid_t : uint8_t
{
    PING = 1,
    CUTDOWN = 2,
    DATA = 3,
    REGULAR = 4,
    ACKREC = 5,
    ACKSEND = 6
};

enum param_t : uint8_t
{
    LATLON = 0,
    TEMPC = 1,
    TEMPK = 2,
    PRESSURE = 3,
    TIME = 4,
    ALT = 5
};

class CommandHandler_t
{
public:
    std::shared_ptr<semaphore_t> dispatchSem = std::make_shared<semaphore_t>();
    std::shared_ptr<semaphore_t> commandSem = std::make_shared<semaphore_t>();
    std::shared_ptr<LimitedQueue_t<uint32_t>> commandQueue = std::make_shared<LimitedQueue_t<uint32_t>>(100);
    std::shared_ptr<LimitedQueue_t<std::string>> dispatchQueue = std::make_shared<LimitedQueue_t<std::string>>(100);
    int ackRec = 0;
    int cmdSent = 0;

    float tempC;
    float tempK;
    float pressure;
    float lat;
    float lon;
    float time;
    float alt;

    CommandHandler_t();

    void addCommand(uint32_t cmd_);

    void addDispatch(std::string dispatch);

    bool dispatchAvailable();

    bool commandAvailable();

    std::string getDispatch();

    std::string createDispatch(sid_t cmd_, param_t param_);

    uint32_t getCommand();

    void executeCommand(uint32_t cmd_);

    bool validateCommand(std::string cmd_, uint32_t *scmd_);

    uint32_t sanitizeCommand(std::string cmd_);

    sid_t getSid(uint32_t cmd_);

    param_t getParam(uint32_t cmd_);

    static void activateCutdown()
    {
        gpio_put(6, 1);
        return;
    };

    void updateDispatchData(float tempC_, float tempK_, float pressure_, float lat_, float lon_, float time_, float alt_);

    int getRecCount();

    std::string createBeaconPayload();

    std::string createSDCardPayload();
};