//
// Created by Mohammad Ashraf Hossain on 7/6/2024.
//

#include "SocketEventHandler.h"
#include "../common/SocketDetails.h"
#include <sys/eventfd.h>


SocketEventHandler::SocketEventHandler() {
    m_pollEvent =  EPOLLIN | EPOLLET | EPOLLRDHUP;
    createEpoll();
    addEventSocket();
}

SocketEventHandler::~SocketEventHandler() {

}

void SocketEventHandler::setEPollEventFlag(uint32_t epollEvent)
{
    m_pollEvent = epollEvent;
}

bool SocketEventHandler::addEventSocket(){
    int eventFd = eventfd(0, EFD_NONBLOCK);
    if (eventFd == -1) {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }
    m_realtimeEventHandleStorePointer = new EventStorePointer();
    m_realtimeEventHandleStorePointer->m_socketId = eventFd;
    addSocket(m_realtimeEventHandleStorePointer);
}

void SocketEventHandler::createEpoll()
{
    m_epollFileDescriptor = epoll_create(MAX_EVENTS);
    if (m_epollFileDescriptor == -1)
    {
        m_continuePolling = false;
        return;
    }
    m_continuePolling = true;


}


void SocketEventHandler::closeEpoll()
{
    m_continuePolling = false;
    close(m_epollFileDescriptor);
}


bool SocketEventHandler::addSocket(EventStorePointer* eventStorePointer) {
    m_pollMutex.lock();
    struct epoll_event ev;
    ev.events = m_pollEvent;
    ev.data.ptr = eventStorePointer;
    if (epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_ADD, eventStorePointer->m_socketId, &ev) == -1) {
        std::cout << "epoll_ctl: listen_sock failed" << std::endl;
        perror("epoll_ctl: listen_sock socket add");
        m_pollMutex.unlock();
        return false;
    }
    eventStorePointer->m_socketEventHandler = this;
    std::cout << eventStorePointer->m_socketId << " Added" << std::endl;
    m_pollMutex.unlock();
    return true;
}

bool SocketEventHandler::removeSocket(EventStorePointer* eventStorePointer) {
    m_pollMutex.lock();
    struct epoll_event ev;
    if (epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_DEL, eventStorePointer->m_socketId, &ev) == -1) {
        perror("epoll_ctl: listen_sock remove socket");
        std::cout << eventStorePointer->m_socketId << " remove error" << std::endl;
        m_pollMutex.unlock();
        return false;
    }
    m_pollMutex.unlock();
    return true;
}

//bool SocketEventHandler::removeSocket(int socketId) {
//    m_pollMutex.lock();
//    struct epoll_event ev;
//    if (epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_DEL, socketId, &ev) == -1) {
//        perror("epoll_ctl: listen_sock");
//        m_pollMutex.unlock();
//        return false;
//    }
//    m_pollMutex.unlock();
//    return true;
//}

void SocketEventHandler::setEventDispatcherPtr(EventDispatcher* eventDispatcher)
{
    m_eventDispatcher = eventDispatcher;
}

void SocketEventHandler::setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler){
    m_removeSocketEventHandler = removeSocketEventHandler;
}

void SocketEventHandler::startPolling(){
    while (m_continuePolling){
        int event_count = epoll_wait(m_epollFileDescriptor, m_events, MAX_EVENTS, 10000);
        if(event_count <= 0 ){
            continue;
        }
        std::cout << "count: "<< event_count << std::endl;
        for (int i = 0; i < event_count; i++){
            EventStorePointer* eventStorePointer = static_cast<EventStorePointer*> (m_events[i].data.ptr);
            if(eventStorePointer->m_socketId == m_realtimeEventHandleStorePointer->m_socketId){
                std::cout << "stopPolling event received" << std::endl;
                continue;
            }

            if (m_events[i].events & EPOLLRDHUP){
                std::cout << "Socket closed" << std::endl;
                if(m_removeSocketEventHandler != NULL)
                    m_removeSocketEventHandler->removeSocket(eventStorePointer);
            }else if(eventStorePointer != NULL) {
                handleEvent(eventStorePointer);
            }
        }
    }
    std::cout << "exited from polling: " << std::endl;
}

void SocketEventHandler::stopPolling()
{
    if(!m_continuePolling)
        return;
    std::cout << "stopPolling" << std::endl;
    m_continuePolling = false;
    uint64_t u = 1;
    int isent = write(m_realtimeEventHandleStorePointer->m_socketId, &u, sizeof(uint64_t));

    removeSocket(m_realtimeEventHandleStorePointer);
    close(m_realtimeEventHandleStorePointer->m_socketId);
    delete m_realtimeEventHandleStorePointer;
}

void SocketEventHandler::handleEvent(EventStorePointer* eventStorePointer)
{
    if(eventStorePointer == NULL){
        return;
    }

    if(m_eventDispatcher != NULL){
        eventStorePointer->m_eventType = EventTypeIncomingData;
        m_eventDispatcher->handleIOEvent(eventStorePointer);
    }
}
