//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"
#include "SocketOperationsHandler.h"


class TLSSocketHandler : public SocketOperationsHandler {
    TLSSocketHandler();
    ~TLSSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer);
    ssize_t sendData(EventStorePointer* eventStorePointer, std::string& data);
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data);
    ssize_t receivePartialData(EventStorePointer* eventStorePointer,int dataSize, std::string& data);
    virtual ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer) = 0;
    void closeConnection();
};



