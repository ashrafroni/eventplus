#include <iostream>
#include "socket/TcpServerSocket.h"
#include "socket/TcpClientSocket.h"
#include "IOWorkerThread/IOWorkerThreadHandler.h"
#include "datatrasaction/TCPSocketHandler.h"
#include<chrono>
#include<thread>



class Ctest : public EventReceiver{
public:
    void newConnectionEvent(EventStorePointer* eventStorePointer){
        std::cout << "newConnectionEvent:" << std::endl;
    }

    void connectionClosedEvent(EventStorePointer* eventStorePointer){
        std::cout << "connectionClosedEvent:" << std::endl;
    }

    void dataEvent(EventStorePointer* eventStorePointer){
        int availabledata = eventStorePointer->getAvailableDataInSocket();
        std::cout << "EventReceiver:" << availabledata << std::endl;
        std::string incomingData;
        eventStorePointer->receiveData(incomingData);
        std::cout << "data:" << incomingData << std::endl;
    }
};

Ctest g_test;




int main() {
    unsigned int numCores = std::thread::hardware_concurrency();


    if (numCores == 0) {
        std::cout << "Number of cores could not be determined." << std::endl;
        numCores = 2;
    } else {
        std::cout << "Number of cores available: " << numCores << std::endl;
    }


    //Destructor and closing all the socket.
    TCPSocketHandler* tcpSocketHandler = new TCPSocketHandler();
    TcpServerSocket tcpServerSocket("127.0.0.1",8089,numCores);
    tcpServerSocket.setSocketOperationHandler(tcpSocketHandler);
    tcpServerSocket.startReceivingConnection();
    tcpServerSocket.createServerSocketAndStartReceiving();
    tcpServerSocket.setEventReceiver(&g_test);


    std::cout<< "Waiting before sent:" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TcpClientSocket tcpClientSocket("127.0.0.1",8089);
    tcpClientSocket.createClientSocketAndStartReceiving();


    std::cout<< "Waiting before sent:" << std::endl;
    std::cout<< "sending:" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    tcpClientSocket.sendData();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    tcpClientSocket.closeSocket();
    tcpServerSocket.stopPolling();
    while(true){
        std::cout<< "Waiting:" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
