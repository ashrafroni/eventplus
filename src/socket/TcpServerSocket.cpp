//
// Created by Ashraf on 7/4/2024.
//


#include <arpa/inet.h>
#include "TcpServerSocket.h"
#include <sys/socket.h>
#include <unistd.h>




TcpServerSocket::TcpServerSocket(const std::string &serverIp, int serverPort,int numCoresInProcessor)
    : m_serverIP(serverIp), m_serverPort(serverPort),m_eventScheduler(numCoresInProcessor){
    m_socketEventHandler.setEventDispatcherPtr(this);
}

const std::string &TcpServerSocket::getServerIp() const {
    return m_serverIP;
}

void TcpServerSocket::setServerIp(const std::string &serverIp) {
    m_serverIP = serverIp;
}

int TcpServerSocket::getServerPort() const {
    return m_serverPort;
}

void TcpServerSocket::setServerPort(int serverPort) {
    m_serverPort = serverPort;
}


void TcpServerSocket::socketAcceptThread(){
    m_socketEventHandler.startPolling();
}

void TcpServerSocket::closeServerSocket(){
    if (m_serverSocketID != -1) {
        m_socketEventHandler.removeSocket(m_eventStore.get());
        close(m_serverSocketID);
    }
}

void TcpServerSocket::handleConnectionEvent(EventStorePointer* eventStorePointer)
{
    //Secured area
    std::unique_lock<std::mutex> lock(clientEventStoresMutex);
    SocketDetails clientSocketDetails;
    socklen_t len = sizeof(clientSocketDetails.m_eventSourceAddress);
    int clientSocketId = accept(eventStorePointer->m_socketId, (struct sockaddr*)&clientSocketDetails.m_eventSourceAddress, &len);
    char ipBuffer[INET_ADDRSTRLEN];
    const char* ipStr = inet_ntop(AF_INET, &(clientSocketDetails.m_eventSourceAddress.sin_addr), ipBuffer, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientSocketDetails.m_eventSourceAddress.sin_port);
    std::cout << "From IP: "<< ipStr << " Client IP:" << ipStr <<  " Socket ID:" << clientSocketId  << " Port:" << clientPort << std::endl;


    //Storing in the map
    auto clientEventStore = std::make_unique<EventStorePointer>(clientSocketDetails);
    clientEventStore->m_socketId = clientSocketId;
    clientEventStore->m_strClientSocketAddress = ipStr;
    clientEventStore->m_eventSourcePort = clientPort;
    clientEventStores[clientSocketId] = std::move(clientEventStore);


    //Adding in the epoll
    EventStorePointer* rawPointer = clientEventStores[clientSocketId].get();
    m_eventScheduler.addSocket(rawPointer);
}

void TcpServerSocket::removeSocket(EventStorePointer* eventStorePointer){
    std::unique_lock<std::mutex> lock(clientEventStoresMutex);
    int clientSocketId = eventStorePointer->m_socketId;

    SocketEventHandler* socketEventHandler = static_cast<SocketEventHandler*>(eventStorePointer->m_socketEventHandler);
    if(socketEventHandler != NULL){
        socketEventHandler->removeSocket(eventStorePointer);
        close(clientSocketId);
    }

    clientEventStores.erase(clientSocketId);
}



bool TcpServerSocket::createServerSocketAndStartReceiving()
{
    int socketId = m_socketHandler.createServerSocket(m_serverIP,std::to_string(m_serverPort),m_socketDetails);
    if(socketId < 0)
        return false;
    m_eventStore = std::make_unique<EventStorePointer>(m_socketDetails);
    m_socketEventHandler.addSocket(m_eventStore.get());
    m_socketEventHandler.startEventReceiverThread();
    return true;
}



