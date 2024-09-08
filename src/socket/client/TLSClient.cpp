//
// Created by Dell on 8/24/2024.
//


#include "TLSClient.h"
#include "../../datatrasaction/client/TLSClientSocketHandler.h"


TLSClient::TLSClient(const std::string& address, const std::string& port)
        : m_address(address), m_port(port) {
    m_baseSocketHandler = new TLSClientSocketHandler();
    m_socketEventHandler = std::make_unique<EventHandlerThread>();
}

TLSClient::~TLSClient() {
}



void TLSClient::handleIOEvent(EventStorePointer* eventStorePointer){
    if(m_clientEventReceiver != nullptr && eventStorePointer != nullptr){
        m_clientEventReceiver->dataEvent(eventStorePointer);
    }
}

void TLSClient::removeSocket(EventStorePointer* eventStorePointer){
    this->Close();
    if(m_clientEventReceiver != nullptr && eventStorePointer != nullptr){
        m_clientEventReceiver->socketRemoved(eventStorePointer);
    }
}

bool TLSClient::Connect() {
    m_socketDetails.m_socketId = m_socketHandler.createClientSocket(m_address, m_port,false);

    if (m_socketDetails.m_socketId < 0) {
        return false;
    }

    m_clientSocket = std::make_unique<EventStorePointer>(m_socketDetails);

    std::cout << "TLSClient::Connect()" << std::endl;
    if (!m_baseSocketHandler->initConnection(m_clientSocket.get())) {
        std::cout << "TLSClient::Connect() ret false" << std::endl;
        return false;
    }
    std::cout << "TLSClient::Connect() ok" << std::endl;
    m_socketHandler.setNonBlocking(m_clientSocket->m_socketId);
    if (!m_socketEventHandler->addSocket(m_clientSocket.get())) {
        return false;
    }

    m_socketEventHandler->startEventReceiverThread();
    std::cout << "m_socketEventHandler->startEventReceiverThread ok" << std::endl;
    return true;
}

int TLSClient::SendData(std::string& data) {
    return m_baseSocketHandler->sendData(m_clientSocket.get(),data);
}



void TLSClient::Close() {
//    m_socketHandler.setBlocking(m_clientSocket->m_socketId);
    bool bIsRemovedFromPoll = m_socketEventHandler->removeSocket(m_clientSocket.get());
    m_socketHandler.setBlocking(m_clientSocket->m_socketId);
    m_baseSocketHandler->closeConnection(m_clientSocket.get());

}