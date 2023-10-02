#include "../include/CommandHandler.h"

CommandHandler_t::CommandHandler_t()
{
    sem_init(dispatchSem.get(), 1, 1);
    sem_init(commandSem.get(), 1, 1);
};

void CommandHandler_t::addCommand(uint32_t cmd_)
{
    // Add command to queue
    sem_acquire_blocking(commandSem.get());
    commandQueue->push(cmd_);
    sem_release(commandSem.get());
};

void CommandHandler_t::addDispatch(std::string dispatch)
{
    // Add dispatch to queue
    sem_acquire_blocking(dispatchSem.get());
    dispatchQueue->push(dispatch);
    sem_release(dispatchSem.get());
};

bool CommandHandler_t::dispatchAvailable()
{
    return dispatchQueue->isEmpty();
};

bool CommandHandler_t::commandAvailable()
{
    return commandQueue->isEmpty();
};

std::string CommandHandler_t::getDispatch()
{
    sem_acquire_blocking(dispatchSem.get());
    std::string dispatch = dispatchQueue->pop();
    sem_release(dispatchSem.get());
    return dispatch;
};

uint32_t CommandHandler_t::getCommand()
{
    sem_acquire_blocking(commandSem.get());
    uint32_t cmd = commandQueue->pop();
    sem_release(commandSem.get());
    return cmd;
};

std::string CommandHandler_t::createDispatch(sid_t cmd_, uint8_t param_)
{
    // Dispatch to be constructed
    char dispatch[30] = "";

    switch (cmd_)
    {
    case PING:
        printf("Ping Dispatch Added");
        snprintf(dispatch, 30, "I'm alive");
        break;
    case CUTDOWN:
        printf("Cutdown Dispatch Added");
        snprintf(dispatch, 30, "Cutdown command");
        break;
    case DATA:
        printf("Data Dispatch Added");
        snprintf(dispatch, 30, "heres data");
        break;
    case REGULAR:
        printf("Regular Dispatch Added");
        snprintf(dispatch, 30, "I'm data");
        break;

    case ACKREC:
        break;

    case ACKSEND:
        printf("Ack dispatch Added");
        snprintf(dispatch, 30, "ack: %d", param_);
        break;
    default:
        return "";
        break;
    }
    printf("\n");
    return dispatch;
};

void CommandHandler_t::executeCommand(uint32_t cmd_)
{

    uint16_t callsign = this->getCallsign(cmd_);
    sid_t sid = this->getSid(cmd_);
    uint8_t param = this->getParam(cmd_);

    switch (sid)
    {
    case PING:
        this->addDispatch(this->createDispatch(sid, param));
        break;
    case CUTDOWN:
        CommandHandler_t::activateCutdown();
        this->addDispatch(this->createDispatch(sid, param));
        break;
    case DATA:
        this->addDispatch(this->createDispatch(sid, param));
        break;
    case REGULAR:
        this->addDispatch(this->createDispatch(sid, param));
        break;
    case ACKREC:
        this->ackRec++;
        break;
    case ACKSEND:
        this->addDispatch(this->createDispatch(sid, param));
        break;

    default:
        printf("Invalid Command\n");
        break;
    }
    return;
};

bool CommandHandler_t::validateCommand(std::string cmd_)
{
    // Check length is 32 bits
    if (cmd_.length() != 4)
    {
        printf("Command received has invalid length\n");
        return false;
    }

    uint32_t sanitizedCommand = this->sanitizeCommand(cmd_);

    uint16_t callsign = this->getCallsign(sanitizedCommand);

    // Check if callsign matches
    if (CALLSIGN != callsign)
    {
        printf("Invalid callsign got: %d expected %d\n", callsign, CALLSIGN);
        return false;
    }

    // Check if command supported
    sid_t sid = this->getSid(sanitizedCommand);
    bool supportedCommand = false;

    for (int i = PING; i <= ACKSEND; i++)
    {
        if (i == sid)
        {
            supportedCommand = true;
        }
    }

    if (!supportedCommand)
    {
        printf("Sid %d is not supported\n", sid);
    }

    // Command is supported, send acknowledge
    uint32_t ackCmd = (CALLSIGN << 16) | (ACKSEND << 8) | (sid);
    this->addCommand(ackCmd);

    return true;
}

uint32_t CommandHandler_t::sanitizeCommand(std::string cmd_)
{
    uint32_t sanitizedCommand = 0;
    for (int i = 0; i < 4; i++)
    {
        char c = cmd_[i];
        uint8_t converted = c - '0';
        sanitizedCommand = sanitizedCommand | (converted << 8 * (3 - i));
    }

    return sanitizedCommand;
}

uint16_t CommandHandler_t::getCallsign(uint32_t cmd_)
{
    uint16_t callsign = (cmd_ & CALLSIGN_BITMASK) >> 16;
    return callsign;
}

sid_t CommandHandler_t::getSid(uint32_t cmd_)
{
    sid_t sid = static_cast<sid_t>(((cmd_ & SID_BITMASK) >> 8));
    return sid;
}

uint8_t CommandHandler_t::getParam(uint32_t cmd_)
{
    uint8_t param = (cmd_ & PARAM_BITMASK);
    return param;
}