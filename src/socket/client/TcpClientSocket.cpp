//
// Created by Mohammad Ashraf Hossain on 7/21/2024.
//

#include <netdb.h>
#include <cstring>
#include <string>
#include "TcpClientSocket.h"


TcpClientSocket::TcpClientSocket(const std::string &serverIp, int serverPort): m_serverIP(serverIp), m_serverPort(serverPort){
    m_clientSocket = std::make_unique<EventStorePointer>();
}


bool TcpClientSocket::createClientSocketAndStartReceiving(){
    int clientSocket = m_socketHandler.createClientSocket(m_serverIP,std::to_string(m_serverPort));

    if(clientSocket < 0)
        return false;

    m_socketDetails.m_socketId = clientSocket;
    m_clientSocket = std::make_unique<EventStorePointer>(m_socketDetails);

    if(m_socketEventHandler == nullptr){
        m_socketEventHandler = std::make_unique<EventHandlerThread>();
    }

    m_socketEventHandler->addSocket(m_clientSocket.get());
    m_socketEventHandler->startEventReceiverThread();
}


void TcpClientSocket::handleIOEvent(EventStorePointer* eventStorePointer){
    std::string data;
    eventStorePointer->receiveData(data);
    std::cout << data << std::endl;
}

bool TcpClientSocket::sendData(){
    std::string data = "test data 1 2 3 45";
    ssize_t bytes_sent = send(m_socketDetails.m_socketId, data.c_str(), data.size(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Error sending data" << std::endl;
        return false;
    }
    std::cout << bytes_sent << " byte data sent " << std::endl;
    return true;
}

void TcpClientSocket::socketAcceptThread(){
    m_socketEventHandler->startPolling();
}
bool TcpClientSocket::closeSocket(){
    m_socketEventHandler->stopPolling();
    close(m_socketDetails.m_socketId);
    return true;
}


const std::string &TcpClientSocket::getServerIp() const{
    return m_serverIP;
}
void TcpClientSocket::setServerIp(const std::string &serverIp){
    m_serverIP = serverIp;
}



void TcpClientSocket::setSocketEventHandler(const std::unique_ptr<EventHandlerThread> &socketEventHandler) {
//    m_socketEventHandler = socketEventHandler;
}
