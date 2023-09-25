#include "pico/sync.h"
#include <memory>
#include <string>
#include "./LimitedQueue.h"
#define CALLSIGN 0x0101
#define CALLSIGN_BITMASK 0xFFFF0000
#define SID_BITMASK 0x0000FF00
#define PARAM_BITMASK 0x000000FF
#define REGULAR_TRANSMISSION 0xFFFF0400

enum sid_t : uint8_t
{
    PING = 1,
    CUTDOWN = 2,
    DATA = 3,
    REGULAR = 4,
    ACKREC = 5,
    ACKSEND = 6
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
    signed int pressure;
    float lat;
    float lon;

    CommandHandler_t();

    void addCommand(uint32_t cmd_);

    void addDispatch(std::string dispatch);

    bool dispatchAvailable();

    bool commandAvailable();

    std::string getDispatch();

    std::string createDispatch(sid_t cmd_, uint8_t param_);

    uint32_t getCommand();

    void executeCommand(uint32_t cmd_);

    bool validateCommand(std::string cmd_);

    uint32_t sanitizeCommand(std::string cmd_);

    uint16_t getCallsign(uint32_t cmd_);

    sid_t getSid(uint32_t cmd_);

    uint8_t getParam(uint32_t cmd_);

    static void activateCutdown()
    {
        return;
    };
};