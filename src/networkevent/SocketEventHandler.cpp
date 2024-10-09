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
    addEventPipeInEpoll();

}

SocketEventHandler::~SocketEventHandler() {

}

void SocketEventHandler::setEPollEventFlag(uint32_t epollEvent)
{
    m_pollEvent = epollEvent;
}



void SocketEventHandler::createEpoll(){
    m_epollFileDescriptor = epoll_create(MAX_EVENTS);
    if (m_epollFileDescriptor == -1){
        m_continuePolling = false;
        return;
    }
    m_continuePolling = true;
}


void SocketEventHandler::closeEpoll(){
    if(m_epollFileDescriptor == -1)
        return;
    m_continuePolling = false;
    //Closing the pipe FD
    close(m_pipeFds[0]);
    close(m_pipeFds[1]);
    close(m_epollFileDescriptor);
    m_epollFileDescriptor = -1;
    m_pipeFds[0] = m_pipeFds[1] = -1;
    std::cout << "Epoll Socket closed" << std::endl;
}


bool SocketEventHandler::addSocket(EventStorePointer* eventStorePointer) {

    std::lock_guard<std::mutex> lock(m_pollMutex);
    struct epoll_event ev{};
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
    struct epoll_event ev{};
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
    std::cout << "startPolling:"  << std::endl;
    while (m_continuePolling){
        int event_count = epoll_wait(m_epollFileDescriptor, m_events, MAX_EVENTS, 10000);
        if(event_count < 1 ){
            continue;
        }

        for (int i = 0; i < event_count; i++){

            if (m_events[i].data.fd == m_pipeFds[0]) {
                char stopSignal;
                read(m_pipeFds[0], &stopSignal, sizeof(stopSignal));  // Clear the event
                std::cout << "Received stop signal, exiting polling..." << std::endl;
               break;
            }


            auto* eventStorePointer = static_cast<EventStorePointer*> (m_events[i].data.ptr);
            if (m_events[i].events & EPOLLRDHUP){
                if(m_removeSocketEventHandler != nullptr)
                    m_removeSocketEventHandler->removeSocket(eventStorePointer);
            }else if(m_events[i].events & EPOLLIN){
                eventStorePointer->m_eventType = EventTypeIncomingData;
                handleEvent(eventStorePointer);
            }else if(eventStorePointer != nullptr) {
                eventStorePointer->m_eventType = EventTypeOther;
                handleEvent(eventStorePointer);
            }
        }
    }
    closeEpoll();
    std::cout << "exited from polling: " << std::endl;
}

void SocketEventHandler::stopPolling(){
    if(m_epollFileDescriptor == -1){
        std::cout << "Socket already closed" << std::endl;
        return;
    }
    m_continuePolling = false;
    if(m_pipeFds[1] > 0){
        char stopSignal = 'x';
        write(m_pipeFds[1], &stopSignal, sizeof(stopSignal));
    }
}

void SocketEventHandler::handleEvent(EventStorePointer* eventStorePointer){
    if(!m_continuePolling)
        return;

    if(eventStorePointer == nullptr){
        return;
    }
    if(m_eventDispatcher != nullptr  ){
        m_eventDispatcher->handleIOEvent(eventStorePointer);
    }
}

bool SocketEventHandler::addEventPipeInEpoll(){
    if (pipe(m_pipeFds) == -1) {
        perror("pipe");
        return false;
    }

    // Add the read end of the pipe to the epoll instance
    struct epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = m_pipeFds[0];  // Add the read end of the pipe
    epoll_ctl(m_epollFileDescriptor, EPOLL_CTL_ADD, m_pipeFds[0], &event);
    return true;
}

