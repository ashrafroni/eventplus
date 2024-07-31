#include <iostream>
#include "socket/TcpServerSocket.h"
#include "socket/TcpClientSocket.h"
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

    TcpServerSocket tcpServerSocket("127.0.0.1",8089,numCores);
    tcpServerSocket.createServerSocketAndStartReceiving();


    TcpClientSocket tcpClientSocket("127.0.0.1",8089);
    tcpClientSocket.createClientSocketAndStartReceiving();
//    tcpClientSocket.sendData();
    while(true){
        std::cout<< "Waiting:" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
