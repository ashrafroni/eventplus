//
// Created by Mohammad Ashraf Hossain on 7/6/2024.
//

#pragma once

#include <netinet/in.h>
#include <sys/epoll.h>
#include "CommonDefinition.h"
#include "SocketOperationsHandler.h"

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
    EventType m_eventType;
    SocketOperationsHandler* m_socketOperationHandler;
    std::mutex m_socketMutex;

//    SSL* m_pSSL;
    EventStorePointer() : m_parameters(nullptr),m_socketEventHandler(nullptr),m_socketOperationHandler(nullptr),m_eventType(EventTypeBlank) {
    }

    EventStorePointer(const SocketDetails& details, void* parameters = nullptr)
            : SocketDetails(details), m_parameters(parameters),m_socketEventHandler(nullptr),m_socketOperationHandler(nullptr),m_eventType(EventTypeBlank) {

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

    void setSocketHandler(SocketOperationsHandler* socketOperationsHandler) {
        m_socketOperationHandler = socketOperationsHandler;
    }

    bool initConnection() {
        return m_socketOperationHandler->initConnection(this);
    }

    ssize_t sendData(std::string& data) {
        return m_socketOperationHandler->sendData(this, data);
    }

    ssize_t receiveData(std::string& data) {
        return m_socketOperationHandler->receiveData(this, data);
    }

    ssize_t receivePartialData(int dataSize, std::string& data) {
        return m_socketOperationHandler->receivePartialData(this, dataSize, data);
    }

    ssize_t getAvailableDataInSocket() {
        return m_socketOperationHandler->getAvailableDataInSocket(this);
    }

    void closeConnection() {
        m_socketOperationHandler->closeConnection(this);
    }
};