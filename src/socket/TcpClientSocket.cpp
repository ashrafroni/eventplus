//
// Created by Dell on 7/21/2024.
//

#include <netdb.h>
#include <cstring>
#include "TcpClientSocket.h"


TcpClientSocket::TcpClientSocket(const std::string &serverIp, int serverPort): m_serverIP(serverIp), m_serverPort(serverPort){
    m_eventStore = std::make_unique<EventStorePointer>();
}


bool TcpClientSocket::createClientSocketAndStartReceiving(){
    int clientSocket = m_socketHandler.createClientSocket(m_serverIP,std::to_string(m_serverPort));

    if(clientSocket < 0)
        return false;

    m_socketDetails.m_socketId = clientSocket;

    m_eventStore = std::make_unique<EventStorePointer>(m_socketDetails);

    m_socketEventHandler.addSocket(m_eventStore.get());

    m_socketEventHandler.startEventReceiverThread();
}

bool TcpClientSocket::sendData(){
    std::string data = "test";
    ssize_t bytes_sent = send(m_socketDetails.m_socketId, data.c_str(), data.size(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Error sending data" << std::endl;
        return false;
    }
    std::cout << bytes_sent << " byte data sent " << std::endl;
    return true;
}

void TcpClientSocket::socketAcceptThread(){
    m_socketEventHandler.startPolling();
}


const std::string &TcpClientSocket::getServerIp() const{
    return m_serverIP;
}
void TcpClientSocket::setServerIp(const std::string &serverIp){

}