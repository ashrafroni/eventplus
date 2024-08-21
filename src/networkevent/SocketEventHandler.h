//
// Created by Ashraf on 7/6/2024.
//

#pragma once
#define MAX_EVENTS 5000

#include <sys/epoll.h>
#include <mutex>
#include <memory>
#include <unistd.h>
#include "../common/EventDispatcher.h"
#include "../common/SocketDetails.h"
#include "../common/CommonDefinition.h"
#include "../common/SocketRemovalHandler.h"

class SocketEventHandler {

public:
    SocketEventHandler();
    ~SocketEventHandler();

    bool addSocket(EventStorePointer* eventStorePointer);
    bool removeSocket(EventStorePointer* eventStorePointer);
    void handleEvent(EventStorePointer* eventStorePointer);
    bool removeSocket(int socketId);

    void startPolling();
    void stopPolling();
    void setEpollEvent(uint32_t events);
    void setEventDispatcherPtr(EventDispatcher* eventDispatcher);
    void setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler);
    void createEpoll();
    void closeEpoll();

    void setEPollEventFlag(uint32_t epollEvent);

private:
//    bool addEventSocket();
//    bool removeEventSocket();

private:
    bool m_continuePolling;
    epoll_event m_epollEvent;
    uint32_t m_pollEvent;
    int m_epollFileDescriptor;
    epoll_event m_events[MAX_EVENTS];
    int m_connectionSource;
    EventDispatcher* m_eventDispatcher;
    SocketRemovalHandler* m_removeSocketEventHandler;
    EventStorePointer* m_realtimeEventHandleStorePointer;
    std::mutex m_pollMutex;
};



