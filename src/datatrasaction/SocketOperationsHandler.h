//
// Created by Dell on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"


class SocketOperationsHandler {
    virtual ~SocketOperationsHandler() = default;

    virtual bool initConnection(std::unique_ptr<EventStorePointer> eventStorePointer) = 0;
    virtual ssize_t sendData(std::unique_ptr<EventStorePointer> eventStorePointer,const std::string& data) = 0;
    virtual ssize_t receiveData(std::unique_ptr<EventStorePointer> eventStorePointer, std::string& data) = 0;
    virtual void closeConnection() = 0;
};



