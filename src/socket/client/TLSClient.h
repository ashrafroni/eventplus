//
// Created by Dell on 8/24/2024.
//


#pragma once

#include <iostream>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include "../SocketHandler.h"
#include "../../networkevent/EventHandlerThread.h"
#include "../../common/ClientEventReceiver.h"

class TLSClient : public EventDispatcher{
public:
    TLSClient(const std::string& address, const std::string& port);
    ~TLSClient();

    bool Connect();
    int SendData(std::string& data);
    void Close();
    void handleIOEvent(EventStorePointer* eventStorePointer) override;

private:
    std::string m_address;
    std::string m_port;

    SocketHandler m_socketHandler;
    SocketDetails m_socketDetails;
    std::unique_ptr<EventStorePointer> m_clientSocket;
    std::unique_ptr<EventHandlerThread> m_socketEventHandler;
    BaseSocketHandler* m_baseSocketHandler;
    ClientEventReceiver* m_clientEventReceiver;
};