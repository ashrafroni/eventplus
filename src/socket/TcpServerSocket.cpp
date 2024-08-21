//
// Created by Ashraf on 7/4/2024.
//

#include "TcpServerSocket.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

TcpServerSocket::TcpServerSocket(const std::string &serverIp, int serverPort,int numCoresInProcessor)
    : m_serverIP(serverIp), m_serverPort(serverPort),m_eventScheduler(numCoresInProcessor){
    m_socketEventHandler.setEventDispatcherPtr(this);
    m_eventScheduler.setSocketRemovalHandler(this);
    ioWorkerThreadHandler = std::make_unique<IOWorkerThreadHandler>(numCoresInProcessor);
    m_callBackFunction = std::bind(&TcpServerSocket::handleCallBackEvent, this, std::placeholders::_1);
    ioWorkerThreadHandler->setTaskFunction(m_callBackFunction);
}

TcpServerSocket::~TcpServerSocket(){

}
void TcpServerSocket::stopPolling(){
    closeServerSocket();
    m_socketEventHandler.stopEventThread();
    m_eventScheduler.stopAllPollingAndThread();
    closeAllClientSocket();
}

void TcpServerSocket::closeAllClientSocket(){

    for(const auto& [socketid,clientSocket] : clientEventStores){
        clientSocket->closeConnection();
    }
    clientEventStores.clear();
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
        if(!m_socketHandler.isSocketClosed(m_serverSocketID)) {
            m_socketEventHandler.removeSocket(m_eventStore.get());
            m_eventStore->closeConnection();
        }

    }
}

void TcpServerSocket::handleIOEvent(EventStorePointer* eventStorePointer)
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

    //Set non blocking
    m_socketHandler.setNonBlocking(clientSocketId);

    //Storing in the map
    auto clientEventStore = std::make_unique<EventStorePointer>(clientSocketDetails);
    clientEventStore->m_socketId = clientSocketId;
    clientEventStore->m_strClientSocketAddress = ipStr;
    clientEventStore->m_eventSourcePort = clientPort;
    clientEventStore->setSocketHandler(m_socketOperationHandler);

    //Adding in the epoll
    EventStorePointer* rawPointer = clientEventStore.get();
    rawPointer->m_eventType = EventTypeNewConnection;
    m_eventScheduler.addSocket(rawPointer);
    clientEventStores[clientSocketId] = std::move(clientEventStore);

    //Add call back thread for
    ioWorkerThreadHandler->handleIOEvent(rawPointer);
}

void TcpServerSocket::removeSocket(EventStorePointer* eventStorePointer){
    std::unique_lock<std::mutex> lock(clientEventStoresMutex);

//    Remove socket from the epoll and assign the removing task to the IO call back thread.
    eventStorePointer->m_eventType = EventTypeClosedConnection;

    int clientSocketId = eventStorePointer->m_socketId;
    auto* socketEventHandlerSocketRemove = static_cast<SocketEventHandler*>(eventStorePointer->m_socketEventHandler);
    if(socketEventHandlerSocketRemove != nullptr){
        socketEventHandlerSocketRemove->removeSocket(eventStorePointer);

        //Close the socket so that no network event comes
        close(clientSocketId);
    }
    //Add call back to back application
    ioWorkerThreadHandler->handleIOEvent(eventStorePointer);
}



bool TcpServerSocket::createServerSocketAndStartReceiving(){
    int socketId = m_socketHandler.createServerSocket(m_serverIP,std::to_string(m_serverPort),m_socketDetails);
    if(socketId < 0)
        return false;
    m_eventStore = std::make_unique<EventStorePointer>(m_socketDetails);
    m_eventStore->setSocketHandler(m_socketOperationHandler);
    m_socketEventHandler.addSocket(m_eventStore.get());
    m_socketEventHandler.startEventReceiverThread();
    return true;
}

void TcpServerSocket::handleCallBackEvent(EventStorePointer* eventStorePointer){
    if(eventStorePointer->m_eventType == EventTypeIncomingData){
        if(m_eventReceiver != nullptr)
            m_eventReceiver->dataEvent(eventStorePointer);
    }else if(eventStorePointer->m_eventType == EventTypeNewConnection){
        if(m_eventReceiver != nullptr)
            m_eventReceiver->newConnectionEvent(eventStorePointer);
    }else if(eventStorePointer->m_eventType == EventTypeClosedConnection){
        //Handle the event in the receiver side
        if(m_eventReceiver != nullptr)
            m_eventReceiver->connectionClosedEvent(eventStorePointer);

        //Remove they map entry so this is locked
        std::unique_lock<std::mutex> lock(clientEventStoresMutex);
        int clientSocketIdToRemove = eventStorePointer->m_socketId;
        auto it = clientEventStores.find(clientSocketIdToRemove);
        if (it != clientEventStores.end()) {
            clientEventStores.erase(it);
        }
    }
}

//void TcpServerSocket::setEventDispatcherForIOEvent(EventDispatcher* eventDispatcher){
//}

void TcpServerSocket::startReceivingConnection(){
    m_eventScheduler.setEventDispatcherPtr(ioWorkerThreadHandler.get());
    m_eventScheduler.startAllEventHandler();
}

void TcpServerSocket::setSocketOperationHandler(SocketOperationsHandler* socketOperationHandler){
    m_socketOperationHandler = socketOperationHandler;
}
void TcpServerSocket::setEventReceiver(EventReceiver* eventReceiver){
    m_eventReceiver = eventReceiver;
}