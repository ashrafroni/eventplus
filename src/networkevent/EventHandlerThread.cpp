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
    stopPolling();
    if (m_eventHandlerThread && m_eventHandlerThread->joinable()) {
        m_eventHandlerThread->join();
    }
    closeEpoll();
    std::cout<< "Thread stopped" << std::endl;
}

EventHandlerThread::EventHandlerThread(){
}


EventHandlerThread::~EventHandlerThread(){
}