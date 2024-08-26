//
// Created by Dell on 8/24/2024.
//


#pragma once

#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <unistd.h>
#include "../SocketHandler.h"
#include "../../networkevent/EventHandlerThread.h"

class TLSClient : public EventDispatcher{
public:
    TLSClient(const std::string& address, const std::string& port);
    ~TLSClient();

    bool Init();
    bool Connect();
    int SendData(const uint8_t* data, int length);
    std::string ReceiveData(int size);
    void Close();
    void handleIOEvent(EventStorePointer* eventStorePointer) override;



private:
    void InitCTX();
    bool CreateSocket();

    std::string m_address;
    std::string m_port;
    SSL_CTX* m_ctx = nullptr;



    SocketHandler m_socketHandler;
    SocketDetails m_socketDetails;
    std::unique_ptr<EventStorePointer> m_clientSocket;
    std::unique_ptr<EventHandlerThread> m_socketEventHandler;
};