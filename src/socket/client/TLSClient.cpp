//
// Created by Dell on 8/24/2024.
//


#include "TLSClient.h"
#include "../../datatrasaction/client/TLSClientSocketHandler.h"
#include <netinet/tcp.h>

#include <utility>

TLSClient::TLSClient(std::string  address, std::string  port)
        : m_address(std::move(address)), m_port(std::move(port)) {
    m_baseSocketHandler = new TLSClientSocketHandler();
    m_socketEventHandler = std::make_unique<EventHandlerThread>();
}

TLSClient::~TLSClient() {
    std::cout << "TLSClient destructor" << std::endl;
}



void TLSClient::handleIOEvent(EventStorePointer* eventStorePointer){
    std::cout << "TLSClient::handleIOEvent" << std::endl;
    if(m_clientEventReceiver != nullptr && eventStorePointer != nullptr){
        m_clientEventReceiver->dataEvent(eventStorePointer);
    }
}

void TLSClient::setClientEventReceiver(ClientEventReceiver* clientEventReceiver){
    m_clientEventReceiver = clientEventReceiver;
}

void TLSClient::removeSocket(EventStorePointer* eventStorePointer){
    this->close();
    if(m_clientEventReceiver != nullptr && eventStorePointer != nullptr){
        m_clientEventReceiver->socketRemoved(eventStorePointer);
    }
}

bool TLSClient::connect() {
    m_socketDetails.m_socketId = m_socketHandler.createClientSocket(m_address, m_port,false);

    if (m_socketDetails.m_socketId < 0) {
        return false;
    }

    m_clientSocket = std::make_unique<EventStorePointer>(m_socketDetails);


    if (!m_baseSocketHandler->initConnection(m_clientSocket.get())) {
        return false;
    }
    m_clientSocket->setSocketHandler(m_baseSocketHandler);
    m_socketHandler.setNonBlocking(m_clientSocket->m_socketId);
    if (!m_socketEventHandler->addSocket(m_clientSocket.get())) {
        return false;
    }

    int flag = 1;
    setsockopt(m_socketDetails.m_socketId, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

    int keepalive = 1;
    setsockopt(m_socketDetails.m_socketId, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));


//    int bufSize = 65536; // 64KB
//    setsockopt(m_socketDetails.m_socketId, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
//    setsockopt(m_socketDetails.m_socketId, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));

//    int optval = 1;
//    setsockopt(m_socketDetails.m_socketId, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


//    SSL_CTX_set_min_proto_version(sslContext, TLS1_2_VERSION);
//    SSL_CTX_set_max_proto_version(sslContext, TLS1_3_VERSION);
//    SSL_CTX_set_session_cache_mode(sslContext, SSL_SESS_CACHE_SERVER);
//    SSL_CTX_set_cipher_list(sslContext, "ECDHE-RSA-AES256-GCM-SHA384");


    m_socketEventHandler->setEventDispatcherPtr(this);
    m_socketEventHandler->setSocketRemovalHandler(this);
    m_socketEventHandler->startEventReceiverThread();
    return true;
}

int TLSClient::sendData(std::string& data) {
    return m_baseSocketHandler->sendData(m_clientSocket.get(),data);
}



void TLSClient::close() {

    if(m_socketEventHandler == nullptr || m_baseSocketHandler == nullptr) {
        return;
    }
    m_socketEventHandler->removeSocket(m_clientSocket.get());
    m_socketEventHandler->stopEventThread();
    std::cout << "stopEventThread()" << std::endl;
    m_socketHandler.setBlocking(m_clientSocket->m_socketId);
    std::cout << "setBlocking()" << std::endl;
    m_baseSocketHandler->closeConnection(m_clientSocket.get());


//    m_clientSocket
    std::cout << "TLSClient::close()" << std::endl;
}