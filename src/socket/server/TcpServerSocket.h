//
// Created by Ashraf on 7/4/2024.
//

#pragma once

#include "../../common/CommonDefinition.h"
#include "../../common/SocketDetails.h"
#include "../../networkevent/SocketEventHandler.h"
#include "../../networkevent/EventHandlerThread.h"
#include "../SocketHandler.h"
#include "../../networkevent/EventScheduler.h"
#include "../../common/SocketRemovalHandler.h"
#include "../../common/ServerEventReceiver.h"
#include "../../IOWorkerThread/IOWorkerThreadHandler.h"
#include <thread>
#include <sys/socket.h>
#include <unordered_map>


class TcpServerSocket : public EventDispatcher,SocketRemovalHandler {

public:
    TcpServerSocket(const std::string &serverIp, int serverPort,int numCoresInProcessor);
    ~TcpServerSocket();

    const std::string &getServerIp() const;
    void setServerIp(const std::string &serverIp);
    int getServerPort() const;
    void setServerPort(int serverPort);

    void closeServerSocket();
    void startReceivingConnection();
    bool createServerSocketAndStartReceiving();

    //Handle new client socket
    void handleIOEvent(EventStorePointer* eventStorePointer) override;

    //Handle Call back event from IOWorker Thread pool
    void handleCallBackEvent(EventStorePointer* eventStorePointer);

    void removeSocket(EventStorePointer* eventStorePointer) override;

    void setSocketOperationHandler(BaseSocketHandler* socketOperationHandler);

    void setEventReceiver(ServerEventReceiver* eventReceiver);

    void stopPolling();
public:
    void socketAcceptThread();
    void closeAllClientSocket();

private:
    void initializeCallBackFunction();

private:
    std::string m_serverIP;
    int m_serverPort;
    int m_serverSocketID;
//    bool m_bIsActive;
    SocketDetails m_socketDetails;
    std::unique_ptr<EventStorePointer> m_eventStore;
    EventHandlerThread m_socketEventHandler;
    SocketHandler m_socketHandler;
    EventScheduler m_eventScheduler;

    std::unordered_map<int, std::unique_ptr<EventStorePointer>> clientEventStores;
    std::mutex clientEventStoresMutex;
    BaseSocketHandler* m_socketOperationHandler;
    ServerEventReceiver* m_eventReceiver;
    std::unique_ptr<IOWorkerThreadHandler> ioWorkerThreadHandler;
    std::function<void(EventStorePointer*)> m_callBackFunction;
};



