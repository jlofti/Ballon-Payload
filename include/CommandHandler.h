#include "pico/sync.h"
#include <memory>
enum cmd_t
{
    PING = 1,
    CUTDOWN = 2,
    DATA = 3,
    REGULAR = 4
};
class CommandHandler_t
{
public:
    std::shared_ptr<struct structsemaphore> dispatchSem(new structsemaphore);
    std::shared_ptr<struct structsemaphore> commandSem(new structsemaphore);
    std::shared_ptr<LimitedQueue_t<cmd_t>> commandQueue;
    std::shared_ptr<LimitedQueue_t<string>> dispatchQueue
    CommandHandler_t()
    {
    }

    void addCommand()
    {
    }

    void addDispatch()
    {
    }

    void dispatchAvailable()
    {
    }

    void commandAvailable()
    {
    }
};