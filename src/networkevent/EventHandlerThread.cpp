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

EventHandlerThread::EventHandlerThread(){
}


EventHandlerThread::~EventHandlerThread(){
}