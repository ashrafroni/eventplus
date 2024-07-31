//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#pragma once


#include "SocketEventHandler.h"

class EventHandlerThread : public SocketEventHandler {
public:
    EventHandlerThread();
    ~EventHandlerThread();

    void startEventReceiverThread();

private:
    void startReceivingEvent();
private:
    std::unique_ptr<std::thread> m_eventHandlerThread;
};



