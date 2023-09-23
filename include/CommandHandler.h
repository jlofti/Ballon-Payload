#include "pico/sync.h"
#include <memory>
#include <string>
#include "./LimitedQueue.h"
enum cmd_t
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
    std::shared_ptr<LimitedQueue_t<cmd_t>> commandQueue = std::make_shared<LimitedQueue_t<cmd_t>>(100);
    std::shared_ptr<LimitedQueue_t<std::string>> dispatchQueue = std::make_shared<LimitedQueue_t<std::string>>(100);
    int ackRec = 0;
    int cmdSent = 0;
    CommandHandler_t();

    void addCommand(cmd_t cmd_);

    void addDispatch(std::string dispatch);

    bool dispatchAvailable();

    bool commandAvailable();

    std::string getDispatch();

    std::string createDispatch(cmd_t cmd_, uint8_t param_);

    cmd_t getCommand();
};