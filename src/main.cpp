#include <iostream>
#include "socket/server/TcpServerSocket.h"
#include "socket/client/TcpClientSocket.h"
#include "IOWorkerThread/IOWorkerThreadHandler.h"
#include "datatrasaction/server/TCPServerSocketHandler.h"
#include "datatrasaction/server/TLSServerSocketHandler.h"
#include "socket/client/TLSClient.h"
#include<chrono>
#include<thread>



class Ctest : public ServerEventReceiver{
public:
    void newConnectionEvent(EventStorePointer* eventStorePointer){
        std::cout << "newConnectionEvent:"  << eventStorePointer->m_socketId<< std::endl;
    }

    void connectionClosedEvent(EventStorePointer* eventStorePointer){
        std::cout << "connectionClosedEvent:" << std::endl;
    }

    void dataEvent(EventStorePointer* eventStorePointer){

        int availabledata = eventStorePointer->getAvailableDataInSocket();
        std::cout << "ServerEventReceiver:" << availabledata << std::endl;


        std::string incomingData;
        eventStorePointer->receiveData(incomingData);
        std::cout << "data:" << incomingData << std::endl;
    }
};


class CClientTest : public ClientEventReceiver{
    void dataEvent(EventStorePointer* eventStorePointer){
        std::cout << "dataEvent: Client" << std::endl;
    }
    void socketRemoved(EventStorePointer* eventStorePointer){

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
TLSClient* tlsClient;
int sendDataUsingTLS(){
    std::string address = "127.0.0.1"; // Replace with your server address
    std::string port = "8089";            // Replace with your server port

    // Step 2: Create an instance of the TLSClient class
//    TLSClient client(address, port);
    tlsClient = new TLSClient(address, port);

    bool isConnected = tlsClient->Connect();


    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Connected with server: " << isConnected << std::endl;
    tlsClient->Close();
    std::cout << "Connection Closed :" << isConnected << std::endl;
    return 0;

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


//    TCPServerSocketHandler* tcpSocketHandler = new TCPServerSocketHandler();
    TcpServerSocket tcpServerSocket("127.0.0.1",8089,numCores);
//    tcpServerSocket.setSocketOperationHandler(tcpSocketHandler);
    tcpServerSocket.setSocketOperationHandler(tlsSocketHandler);

    tcpServerSocket.startReceivingConnection();
    tcpServerSocket.createServerSocketAndStartReceiving();
    tcpServerSocket.setEventReceiver(&g_test);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout<< "Waiting before sent:" << std::endl;
    sendDataUsingTLS();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    //tcpServerSocket.stopPolling();
    while(true){
        std::cout<< "Waiting:" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}


//
//
//// Step 3: Initialize the SSL context
//if (!client.Init()) {
//std::cerr << "Failed to initialize SSL context" << std::endl;
//return -1;
//}
//
//// Step 4: Connect to the TLS server
//if (!client.Connect()) {
//std::cerr << "Failed to connect to the server" << std::endl;
//return -1;
//}
//std::this_thread::sleep_for(std::chrono::seconds(10));
//// Step 5: Send data to the server
//const std::string message = "Hello, TLS server!";
//std::cout << message << std::endl;
//int bytesSent = client.SendData(reinterpret_cast<const uint8_t*>(message.c_str()), message.length());
//if (bytesSent <= 0) {
//std::cerr << "Failed to send data to the server" << std::endl;
//client.Close();
//return -1;
//}
//std::cout << "Sent " << bytesSent << " bytes to the server" << std::endl;
//
//// Step 6: Receive data from the server
//std::string response = client.ReceiveData(1024); // Adjust the buffer size as needed
//if (response.empty()) {
//std::cerr << "Failed to receive data from the server" << std::endl;
//client.Close();
//return -1;
//}
//std::cout << "Received from server: " << response << std::endl;
//
//// Step 7: Close the connection
//client.Close();