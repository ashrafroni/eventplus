//
// Created by Dell on 8/24/2024.
//


#include "TLSClient.h"
#include "../../datatrasaction/client/TLSClientSocketHandler.h"


TLSClient::TLSClient(const std::string& address, const std::string& port)
        : m_address(address), m_port(port) {
    m_baseSocketHandler = new TLSClientSocketHandler();
}

TLSClient::~TLSClient() {
}



void TLSClient::handleIOEvent(EventStorePointer* eventStorePointer){
    if(m_clientEventReceiver != nullptr && eventStorePointer != nullptr){
        m_clientEventReceiver->dataEvent(eventStorePointer);
    }
}

bool TLSClient::Connect() {
    m_socketDetails.m_socketId = m_socketHandler.createClientSocket(m_address,m_port);
    if (m_socketDetails.m_socketId < 0) {
        return false;
    }
    m_clientSocket = std::make_unique<EventStorePointer>(m_socketDetails);
    return m_baseSocketHandler->initConnection(m_clientSocket.get());
}

int TLSClient::SendData(std::string& data) {
    m_baseSocketHandler->sendData(m_clientSocket.get(),data);
}



void TLSClient::Close() {
    m_baseSocketHandler->closeConnection(m_clientSocket.get());
}