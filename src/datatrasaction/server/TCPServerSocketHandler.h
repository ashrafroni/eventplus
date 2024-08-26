//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../../common/CommonDefinition.h"
#include "../../common/SocketDetails.h"
#include "../../common/BaseSocketHandler.h"

class TCPServerSocketHandler : public BaseSocketHandler {
public:
    TCPServerSocketHandler();
    ~TCPServerSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer) override;
    ssize_t sendData(EventStorePointer* eventStorePointer,std::string& data)override;
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data)override;
    ssize_t receivePartialData(EventStorePointer* eventStorePointer,int dataSize, std::string& data)override;
    ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer)override;
    void closeConnection(EventStorePointer* eventStorePointer)override;
};



