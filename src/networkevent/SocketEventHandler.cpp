//
// Created by Mohammad Ashraf Hossain on 7/6/2024.
//

#include "SocketEventHandler.h"
#include "../common/SocketDetails.h"
#include <sys/eventfd.h>
#include <fcntl.h>

SocketEventHandler::SocketEventHandler() {
    m_pollEvent =  EPOLLIN | EPOLLET | EPOLLRDHUP;
    createEpoll();
//    addEventSocket();
}

SocketEventHandler::~SocketEventHandler() {

}

void SocketEventHandler::setEPollEventFlag(uint32_t epollEvent)
{
    m_pollEvent = epollEvent;
}



void SocketEventHandler::createEpoll(){
    m_epollFileDescriptor = epoll_create(MAX_EVENTS);
    if (m_epollFileDescriptor == -1)
    {
        m_continuePolling = false;
        return;
    }
    m_continuePolling = true;
}


void SocketEventHandler::closeEpoll(){
    m_continuePolling = false;
    close(m_epollFileDescriptor);
    m_epollFileDescriptor = -1;
}


bool SocketEventHandler::addSocket(EventStorePointer* eventStorePointer) {

    std::lock_guard<std::mutex> lock(m_pollMutex);
    struct epoll_event ev;
    ev.events = m_pollEvent;
    ev.data.ptr = eventStorePointer;
    if (epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_ADD, eventStorePointer->m_socketId, &ev) == -1) {
        perror("epoll_ctl: listen_sock socket add");
        return false;
    }
    eventStorePointer->m_socketEventHandler = this;
    return true;
}

bool SocketEventHandler::removeSocket(EventStorePointer* eventStorePointer) {
    std::lock_guard<std::mutex> lock(m_pollMutex);
    struct epoll_event ev;
    if (epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_DEL, eventStorePointer->m_socketId, &ev) == -1) {
        int epollStatus = fcntl(m_epollFileDescriptor, F_GETFD);
        fprintf(stderr, "epoll_ctl: listen_sock remove socket %d failed: ", eventStorePointer->m_socketId);
        return false;
    }
    return true;
}


void SocketEventHandler::setEventDispatcherPtr(EventDispatcher* eventDispatcher){
    m_eventDispatcher = eventDispatcher;
}

void SocketEventHandler::setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler){
    m_removeSocketEventHandler = removeSocketEventHandler;
}

void SocketEventHandler::startPolling(){
    while (m_continuePolling){
        int event_count = epoll_wait(m_epollFileDescriptor, m_events, MAX_EVENTS, 500);
        if(event_count < 1 ){
            continue;
        }
        for (int i = 0; i < event_count; i++){
            EventStorePointer* eventStorePointer = static_cast<EventStorePointer*> (m_events[i].data.ptr);
            if (m_events[i].events & EPOLLRDHUP){
                if(m_removeSocketEventHandler != NULL)
                    m_removeSocketEventHandler->removeSocket(eventStorePointer);
            }else if(eventStorePointer != NULL) {
                handleEvent(eventStorePointer);
            }
        }
    }
    std::cout << "exited from polling: " << std::endl;
}

void SocketEventHandler::stopPolling(){
    if(!m_continuePolling)
        return;
    m_continuePolling = false;
}

void SocketEventHandler::handleEvent(EventStorePointer* eventStorePointer){
    if(!m_continuePolling)
        return;

    if(eventStorePointer == NULL){
        return;
    }
    if(m_eventDispatcher != NULL){
        eventStorePointer->m_eventType = EventTypeIncomingData;
        m_eventDispatcher->handleIOEvent(eventStorePointer);
    }
}


//        std::cerr << "Failed to remove socket " << eventStorePointer->m_socketId
//                  << " from epoll instance " << m_epollFileDescriptor << ":EpollStatus: " << epollStatus << std::endl;
//        std::cerr << "errno: " << errno << std::endl;

//if (errno == EINTR){
//std::cout << "signal received" << std::endl;
//break;
//}
//if(eventStorePointer->m_socketId == m_realtimeEventHandleStorePointer->m_socketId){
//std::cout << "stopPolling event received" << std::endl;
//break;
//}


//bool SocketEventHandler::addEventSocket(){
//    int eventFd = eventfd(0, EFD_NONBLOCK);
//    if (eventFd == -1) {
//        perror("eventfd");
//        exit(EXIT_FAILURE);
//    }
//    m_realtimeEventHandleStorePointer = new EventStorePointer();
//    m_realtimeEventHandleStorePointer->m_socketId = eventFd;
//    addSocket(m_realtimeEventHandleStorePointer);
//}

//bool SocketEventHandler::removeEventSocket(){
//    close(m_realtimeEventHandleStorePointer->m_socketId);
//    delete m_realtimeEventHandleStorePointer;
//}