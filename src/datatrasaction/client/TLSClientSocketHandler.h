//
// Created by Dell on 8/26/2024.
//



#pragma once


#include "../../common/BaseSocketHandler.h"

class TLSClientSocketHandler : public BaseSocketHandler {
    TLSClientSocketHandler();
    ~TLSClientSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer) override;
    ssize_t sendData(EventStorePointer* eventStorePointer, std::string& data)override;
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data)override;
    ssize_t receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data)override;
    ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer)override;
    void closeConnection(EventStorePointer* eventStorePointer)override;


private:
    void InitCTX();
    bool Init();

private:
    SSL_CTX* m_ctx = nullptr;

};



