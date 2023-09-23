#include "../include/CommandHandler.h"


CommandHandler_t::CommandHandler_t() {
    sem_init(dispatchSem.get(),1,1);
    sem_init(commandSem.get(),1,1);
}

void CommandHandler_t::addCommand(cmd_t cmd_) {
    enum cmd_t commandToPush;

    //Determine command
    switch(cmd_) {
        case PING:
            commandToPush = PING;
            break;
        case CUTDOWN:
            commandToPush = CUTDOWN;
            break;
        case DATA:
            commandToPush = DATA;
            break;
        case REGULAR:
            commandToPush = REGULAR;
            break;
        case ACKREC:
            commandToPush = ACKREC;
            break;
        case ACKSEND:
            commandToPush = ACKSEND;
            break;
        default:
            return;
            break;
    }
    
    //Add command to queue
    sem_acquire_blocking(commandSem.get());
    commandQueue->push(commandToPush);
    sem_release(commandSem.get());

};

void CommandHandler_t::addDispatch(std::string dispatch) {
    sem_acquire_blocking(dispatchSem.get());
    dispatchQueue->push(dispatch);
    sem_release(dispatchSem.get());
};

bool CommandHandler_t::dispatchAvailable() {
    return dispatchQueue->isEmpty();
};

bool CommandHandler_t::commandAvailable() {
    return commandQueue->isEmpty();
};

std::string CommandHandler_t::getDispatch() {
    std::string dispatch = "";
    sem_acquire_blocking(dispatchSem.get());
    dispatch = dispatchQueue->pop();
    sem_release(dispatchSem.get());
    return dispatch;
};

cmd_t CommandHandler_t::getCommand() {
    cmd_t cmd;
    sem_acquire_blocking(commandSem.get());
    cmd = commandQueue->pop();
    sem_release(commandSem.get());
    return cmd;
}

std::string CommandHandler_t::createDispatch(cmd_t cmd_, uint8_t param_) {
    //Dispatch to be constructed
    char dispatch[30] = "";

    switch(cmd_) {
        case PING:
            printf("Ping Dispatch");
            snprintf(dispatch,30, "I'm alive");
            break;
        case CUTDOWN:
            printf("Cutdown Dispatch");
            snprintf(dispatch,30, "Cutdown command");
            break;
        case DATA:
            printf("Data Dispatch");
            snprintf(dispatch,30, "heres data");
            break;
        case REGULAR:
            printf("Regular Dispatch");
            snprintf(dispatch,30, "I'm data");
            break;

        case ACKREC:
            break;

        case ACKSEND:
            printf("Ack dispatch");
            snprintf(dispatch, 30, "ack: %d", param_);
            break;
        default:
            return "";
            break;        
    }
     printf("\n");
     return dispatch;
}