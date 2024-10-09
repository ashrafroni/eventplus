//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#include "EventHandlerThread.h"


void EventHandlerThread::startReceivingEvent(){

    startPolling();
}

void EventHandlerThread::startEventReceiverThread(){
    m_eventHandlerThread = std::make_unique<std::thread>(&EventHandlerThread::startReceivingEvent, this);
}


void EventHandlerThread::stopEventThread(){
    std::cout << "EventHandlerThread::stopEventThread()" << std::endl;
    stopPolling();

    if (m_eventHandlerThread && m_eventHandlerThread->joinable() && !m_threadHandled) {
        m_eventHandlerThread->detach();
        m_threadHandled = true;
    }
}

EventHandlerThread::EventHandlerThread(){
}


EventHandlerThread::~EventHandlerThread(){
    stopPolling();
    closeEpoll();
    std::cout << "~EventHandlerThread()" << std::endl;

    if (m_eventHandlerThread && m_eventHandlerThread->joinable()&& !m_threadHandled) {
        m_eventHandlerThread->detach();
    }
}

