//
// Created by Dell on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"
#include "SocketOperationsHandler.h"

class NormalSocketHandler : public SocketOperationsHandler {
public:
    NormalSocketHandler();
    ~NormalSocketHandler();

    bool initConnection(std::unique_ptr<EventStorePointer> eventStorePointer);
    ssize_t sendData(std::unique_ptr<EventStorePointer> eventStorePointer,const std::string& data);
    ssize_t receiveData(std::unique_ptr<EventStorePointer> eventStorePointer, std::string& data);
    void closeConnection();
};



