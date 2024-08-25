//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"
#include "../common/SocketOperationsHandler.h"
#include "../ssl/SSLCertificateHandler.h"


class TLSSocketHandler : public SocketOperationsHandler {
public:
    TLSSocketHandler();
    ~TLSSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer);
    ssize_t sendData(EventStorePointer* eventStorePointer, std::string& data);
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data);
    ssize_t receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data);
    ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer);
    void closeConnection(EventStorePointer* eventStorePointer);

    bool loadCertificate(const std::string& certificateFileName, const std::string& keyFileName);
private:
    SSLCertificateHandler* m_sslCertificateHandler{};
};



