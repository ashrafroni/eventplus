//
// Created by Ashraf on 7/21/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include "../common/SocketDetails.h"
#include "../networkevent/SocketEventHandler.h"
#include "../networkevent/EventHandlerThread.h"
#include "SocketHandler.h"
#include <thread>
#include <sys/socket.h>

class TcpClientSocket {
public:
    TcpClientSocket(const std::string &serverIp, int serverPort);

    const std::string &getServerIp() const;
    void setServerIp(const std::string &serverIp);
    int CreateConnection();
    bool createClientSocketAndStartReceiving();
    bool sendData();
    bool closeSocket();

public:
    void socketAcceptThread();

private:
    std::string m_serverIP;
    int m_serverPort;
    SocketHandler m_socketHandler;
    SocketDetails m_socketDetails;
    std::unique_ptr<EventStorePointer> m_eventStore;
    EventHandlerThread m_socketEventHandler;
};



