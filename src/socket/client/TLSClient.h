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

class TLSClient : public EventDispatcher,SocketRemovalHandler{
public:
    TLSClient(std::string  address, std::string  port);
    ~TLSClient();

    bool connect();
    int  sendData(std::string& data);
    void close();
    void handleIOEvent(EventStorePointer* eventStorePointer) override;
    void removeSocket(EventStorePointer* eventStorePointer) override;

    void setClientEventReceiver(ClientEventReceiver* clientEventReceiver);


private:
    std::string m_address;
    std::string m_port;

    SocketHandler m_socketHandler;
    SocketDetails m_socketDetails;
    std::unique_ptr<EventStorePointer> m_clientSocket;
    std::unique_ptr<EventHandlerThread> m_socketEventHandler;
    BaseSocketHandler* m_baseSocketHandler;
    ClientEventReceiver* m_clientEventReceiver{};
};