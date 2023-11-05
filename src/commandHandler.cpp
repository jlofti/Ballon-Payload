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

std::string CommandHandler_t::createDispatch(sid_t cmd_, param_t param_)
{
    // Dispatch to be constructed
    char dispatch[128] = "";
    char payload[100] = "";
    snprintf(dispatch, 9, "%s,", CALLSIGN);

    switch (cmd_)
    {
    case PING:
        printf("Ping Dispatch Added");
        snprintf(payload, 30, "Ping");
        break;
    case CUTDOWN:
        printf("Cutdown Dispatch Added");
        snprintf(payload, 30, "Cutdown");
        break;
    case DATA:
        printf("Data Dispatch Added");

        switch (param_)
        {
        case LATLON:
            snprintf(payload, 30, std::to_string(lat).append(",").append(std::to_string(lon)).c_str());
            break;
        case TEMPC:
            snprintf(payload, 30, std::to_string(tempC).c_str());
            break;
        case TEMPK:
            snprintf(payload, 30, std::to_string(tempK).c_str());
            break;
        case PRESSURE:
            snprintf(payload, 30, std::to_string(pressure).c_str());
            break;
        case TIME:
            snprintf(payload, 30, std::to_string(time).c_str());
            break;
        case ALT:
            snprintf(payload, 30, std::to_string(alt).c_str());
            break;
        default:
            break;
        }

        break;
    case REGULAR:
        printf("Regular Dispatch Added");
        snprintf(payload, 80, createBeaconPayload().c_str());
        break;

    case ACKREC:
        printf("Received an ack");
        ackRec++;
        break;

    case ACKSEND:
        printf("Ack dispatch Added");
        snprintf(payload, 30, "ack: %d", param_);
        break;
    default:
        return "";
        break;
    }

    printf("\n");

    // Append Sid
    strcat(dispatch, std::to_string(cmd_).c_str());
    strcat(dispatch, ",");

    // Append param
    strcat(dispatch, std::to_string(param_).c_str());
    strcat(dispatch, ",");

    // Append payload length and payload
    uint8_t len = std::strlen(payload);
    strcat(dispatch, std::to_string(len).c_str());
    strcat(dispatch, ",");

    strcat(dispatch, payload);
    return dispatch;
};

void CommandHandler_t::executeCommand(uint32_t cmd_)
{

    sid_t sid = this->getSid(cmd_);
    param_t param = this->getParam(cmd_);
    bool needsAck = true;

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
        needsAck = false;
        this->addDispatch(this->createDispatch(sid, param));
        break;
    case ACKREC:
        needsAck = false;
        this->ackRec++;
        break;
    case ACKSEND:
        needsAck = false;
        this->addDispatch(this->createDispatch(sid, param));
        break;

    default:
        printf("Invalid Command got SID: %d, Param: %d\n", sid, param);
        break;
    }

    if (needsAck)
    {
        cmdSent++;
    }
    return;
};

bool CommandHandler_t::validateCommand(std::string cmd_, uint32_t *scmd_)
{
    // Length needs to be correct length
    if (cmd_.length() != (CALLSIGN_LENGTH + SID_LENGTH + PARAM_LENGTH))
    {
        printf("Command received has invalid length of %d\n", cmd_.length());
        return false;
    }

    // Incorrect callsign
    if (cmd_.substr(0, 7).compare(CALLSIGN) != 0)
    {
        printf("Command does not have correct callsign\n");
        return false;
    }

    // Take out callsign
    cmd_.erase(0, 7);

    // Append two 0's at beginning for padding -> room for increasing command size
    cmd_.insert(0, PADDING);

    *scmd_ = this->sanitizeCommand(cmd_);

    // Check if command supported
    sid_t sid = this->getSid(*scmd_);
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
    uint32_t ackCmd = (std::stoi(PADDING) << 16) | (ACKSEND << 8) | (sid);
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

sid_t CommandHandler_t::getSid(uint32_t cmd_)
{
    sid_t sid = static_cast<sid_t>(((cmd_ & SID_BITMASK) >> 8));
    return sid;
}

param_t CommandHandler_t::getParam(uint32_t cmd_)
{
    param_t param = static_cast<param_t>(cmd_ & PARAM_BITMASK);
    return param;
}

void CommandHandler_t::updateDispatchData(float tempC_, float tempK_, float pressure_, float lat_, float lon_, float time_, float alt_)
{
    this->tempC = tempC_;
    this->tempK = tempK_;
    this->pressure = pressure_;
    this->lat = lat_;
    this->lon = lon_;
    this->time = time_;
    this->alt = alt_;
};

int CommandHandler_t::getRecCount()
{
    return cmdSent - ackRec;
}

std::string CommandHandler_t::createBeaconPayload()
{
    std::string payload = "";

    payload.append(std::to_string(lat))
        .append(",")
        .append(std::to_string(lon))
        .append(",")
        .append(std::to_string(alt))
        .append(",")
        .append(std::to_string(time));

    return payload;
}

std::string CommandHandler_t::createSDCardPayload()
{
    std::string payload = "";

    payload.append(this->createBeaconPayload())
        .append(",")
        .append(std::to_string(tempC))
        .append(",")
        .append(std::to_string(tempK))
        .append("\n");

    return payload;
}