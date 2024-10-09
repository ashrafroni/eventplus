#include <iostream>
#include "socket/server/TcpServerSocket.h"
#include "socket/client/TcpClientSocket.h"
#include "IOWorkerThread/IOWorkerThreadHandler.h"
#include "datatrasaction/server/TCPServerSocketHandler.h"
#include "datatrasaction/server/TLSServerSocketHandler.h"
#include "socket/client/TLSClient.h"
#include<chrono>
#include<thread>
#include <sstream>



class Ctest : public ServerEventReceiver{
public:
    void newConnectionEvent(EventStorePointer* eventStorePointer){
        std::cout << "newConnectionEvent:"  << eventStorePointer->m_socketId<< std::endl;
    }

    void connectionClosedEvent(EventStorePointer* eventStorePointer){
        std::cout << "connectionClosedEvent:" << std::endl;
    }

    void dataEvent(EventStorePointer* eventStorePointer){

        eventStorePointer->getAvailableDataInSocket();
        std::cout << "TLS ServerEventReceiver:" <<  std::endl;


        std::string incomingData;
        eventStorePointer->receiveData(incomingData);
        std::cout << "data:" << incomingData << std::endl;
        std::ostringstream oss;
        oss << "data:" << incomingData;
        std::string strData = oss.str();
        eventStorePointer->sendData(strData);

    }
};




Ctest g_test;


void sendDataUsingTCP(){
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TcpClientSocket tcpClientSocket("127.0.0.1",8089);
    tcpClientSocket.createClientSocketAndStartReceiving();



    std::cout<< "sending:" << std::endl;

    tcpClientSocket.sendData();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    tcpClientSocket.closeSocket();

}


int main() {
    unsigned int numCores = std::thread::hardware_concurrency();

    std::string certificationFile = "certification.pem";
    std::string keyFile = "certificatekeyfile.pem";

    if (numCores == 0) {
        std::cout << "Number of cores could not be determined." << std::endl;
        numCores = 2;
    } else {
        std::cout << "Number of cores available: " << numCores << std::endl;
    }



    TLSServerSocketHandler* tlsSocketHandler = new TLSServerSocketHandler;
    bool bIsLoaded = tlsSocketHandler->loadCertificate(certificationFile,keyFile);
    if(!bIsLoaded)
        return -1;

    numCores = 1;


    TcpServerSocket tcpServerSocket("127.0.0.1",8089,numCores);
    tcpServerSocket.setSocketOperationHandler(tlsSocketHandler);

    tcpServerSocket.startReceivingConnection();
    tcpServerSocket.createServerSocketAndStartReceiving();
    tcpServerSocket.setEventReceiver(&g_test);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout<< "Waiting before sent:" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(15));
    while(true){
        std::cout<< "Waiting:" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
    return 0;
}


