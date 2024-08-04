//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"


class SocketOperationsHandler {
public:
    virtual ~SocketOperationsHandler() = default;

    virtual bool initConnection(EventStorePointer* eventStorePointer) = 0;
    virtual ssize_t sendData(EventStorePointer* eventStorePointer,std::string& data) = 0;
    virtual ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data) = 0;
    virtual ssize_t receivePartialData(EventStorePointer* eventStorePointer,int dataSize, std::string& data) = 0;
    virtual ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer) = 0;
    virtual void closeConnection() = 0;
};



