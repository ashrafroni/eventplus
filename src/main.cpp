#include <iostream>
#include "socket/TcpServerSocket.h"
#include "socket/TcpClientSocket.h"
#include "IOWorkerThread/IOWorkerThreadHandler.h"
#include "datatrasaction/NormalSocketHandler.h"
#include<chrono>
#include<thread>

int main() {
    unsigned int numCores = std::thread::hardware_concurrency();


    if (numCores == 0) {
        std::cout << "Number of cores could not be determined." << std::endl;
        numCores = 2;
    } else {
        std::cout << "Number of cores available: " << numCores << std::endl;
    }



    //Server IP port
    //Server port
    //Get Connection event
    //Option to add block IP

    IOWorkerThreadHandler ioWorkerThreadHandler(numCores);
    NormalSocketHandler* normalSocketHandler = new NormalSocketHandler();
//    ioWorkerThreadHandler.setSocketOperationHandler(normalSocketHandler);

    TcpServerSocket tcpServerSocket("127.0.0.1",8089,numCores);
    tcpServerSocket.setEventDispatcherForIOEvent(&ioWorkerThreadHandler);
    tcpServerSocket.setSocketOperationHandler(normalSocketHandler);
    tcpServerSocket.startReceivingConnection();
    tcpServerSocket.createServerSocketAndStartReceiving();

    std::cout<< "Waiting before sent:" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TcpClientSocket tcpClientSocket("127.0.0.1",8089);
    tcpClientSocket.createClientSocketAndStartReceiving();

    std::cout<< "Waiting before sent:" << std::endl;
    std::cout<< "sending:" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    tcpClientSocket.sendData();
    std::this_thread::sleep_for(std::chrono::seconds(5));
//    tcpClientSocket.closeSocket();
//    for(int i = 0; i < 7; i++){
//        //std::cout<< "sending again:" << std::endl;
//        tcpClientSocket.sendData();
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//    }
//    tcpClientSocket.sendData();
    while(true){
        std::cout<< "Waiting:" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
