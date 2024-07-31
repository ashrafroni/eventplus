//
// Created by Mohammad Ashraf Hossain on 7/6/2024.
//

#pragma once

#include <netinet/in.h>
#include <sys/epoll.h>
#include "CommonDefinition.h"

class SocketDetails {

public:
    int m_socketId;
    struct sockaddr_in m_eventSourceAddress;
    std::string m_strClientSocketAddress;
    int m_eventSourcePort;
    int m_pollId;
    SocketType m_eventSocketType;
    SocketStatus m_socketStatus;
    EPOLL_EVENTS m_epollEvent;
};


class EventStorePointer : public SocketDetails {
public:
    void* m_parameters;
    void* m_socketEventHandler;

//    SSL* m_pSSL;
    EventStorePointer() : m_parameters(nullptr),m_socketEventHandler(nullptr) {
    }

    EventStorePointer(const SocketDetails& details, void* parameters = nullptr)
            : SocketDetails(details), m_parameters(parameters) {

    }


    EventStorePointer& operator=(const SocketDetails& details) {
        if (this != &details) {
            m_socketId = details.m_socketId;
            m_eventSourceAddress = details.m_eventSourceAddress;
            m_strClientSocketAddress = details.m_strClientSocketAddress;
            m_eventSourcePort = details.m_eventSourcePort;
            m_pollId = details.m_pollId;
            m_eventSocketType = details.m_eventSocketType;
            m_epollEvent = details.m_epollEvent;
        }
        return *this;
    }
};