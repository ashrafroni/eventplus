//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"
#include "SocketOperationsHandler.h"

class NormalSocketHandler : public SocketOperationsHandler {
public:
    NormalSocketHandler();
    ~NormalSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer);
    ssize_t sendData(EventStorePointer* eventStorePointer,std::string& data);
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data);
    void closeConnection();
};



