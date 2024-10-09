//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#pragma once

#include "../../common/CommonDefinition.h"
#include "../../common/SocketDetails.h"
#include "../../ssl/SSLCertificateHandler.h"
#include "../../common/SocketRemovalHandler.h"

class TLSServerSocketHandler : public BaseSocketHandler {
public:
    TLSServerSocketHandler();
    ~TLSServerSocketHandler();

    bool initConnection(EventStorePointer* eventStorePointer) override;
    ssize_t sendData(EventStorePointer* eventStorePointer, std::string& data)override;
    ssize_t receiveData(EventStorePointer* eventStorePointer, std::string& data)override;
    ssize_t receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data)override;
    ssize_t getAvailableDataInSocket(EventStorePointer* eventStorePointer)override;
    void closeConnection(EventStorePointer* eventStorePointer)override;
    void setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler)override;

    bool loadCertificate(const std::string& certificateFileName, const std::string& keyFileName);
private:

    SocketRemovalHandler* m_removeSocketEventHandler;
    SSLCertificateHandler* m_sslCertificateHandler{};
};



