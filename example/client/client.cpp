//
// Created by Dell on 9/28/2024.
//


#include<iostream>
#include<sstream>
#include "../../src/socket/client/TLSClient.h"




int sendDataUsingTLS(){
    std::string address = "127.0.0.1"; // Replace with your server address
    std::string port = "8089";            // Replace with your server port
    TLSClient* tlsClient;
    // Step 2: Create an instance of the TLSClient class
//    TLSClient client(address, port);
    tlsClient = new TLSClient(address, port);

    bool isConnected = tlsClient->Connect();
    std::cout << "Connected with server: " << isConnected << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    tlsClient->Close();
    std::cout << "Connection Closed :" << isConnected << std::endl;
    return 0;

}



int sendDataUsingTLSv2(){
    std::string address = "127.0.0.1"; // Replace with your server address
    std::string port = "8089";            // Replace with your server port
    TLSClient tlsClient(address, port);
    // Step 2: Create an instance of the TLSClient class
//    TLSClient client(address, port);
    bool isConnected = tlsClient.Connect();
    std::cout << "Connected with server: " << isConnected << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Connection Closed :" << isConnected << std::endl;
    return 0;
}

class CClientTest : public ClientEventReceiver{
    void dataEvent(EventStorePointer* eventStorePointer){
        std::string strData;
        eventStorePointer->receiveData(strData);
        std::cout << "dataEvent: Client" << strData << std::endl;
    }


    void socketRemoved(EventStorePointer* eventStorePointer){
        std::cout << "socketRemoved" << std::endl;
    }
};


void sendData10Times(){
    std::string address = "127.0.0.1"; // Replace with your server address
    std::string port = "8089";            // Replace with your server port
    TLSClient tlsClient(address, port);
    CClientTest clientTest;
    // Step 2: Create an instance of the TLSClient class
//    TLSClient client(address, port);
    bool isConnected = tlsClient.Connect();
    tlsClient.setClientEventReceiver(&clientTest);
    std::cout << "Connected with server: " << isConnected << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for(int i = 0; i < 10; i++){
        std::string strData = "test data event plus";
        std::cout << "sendData:" << tlsClient.SendData(strData) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        tlsClient.SendData(strData);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    tlsClient.Close();
    std::cout << "Connection Closed :" << isConnected << std::endl;
}


int main(){
    std::cout << "Enjoy " << std::endl;
    sendData10Times();
    std::cout << "Enjoy exit" << std::endl;
    return 0;
}