//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#include "EventScheduler.h"



EventScheduler::EventScheduler(size_t numThreads){
    m_numThreads = numThreads;
    m_currentIndex = 0;
    m_socketEventHandlers.reserve(numThreads + 1);
    for (size_t currentIndex = 0; currentIndex < numThreads; currentIndex++) {
        m_socketEventHandlers.push_back(std::make_unique<EventHandlerThread>());
    }
}



EventScheduler::~EventScheduler(){
}

void EventScheduler::stopAllPollingAndThread(){
    for (const auto & socketEventHandler : m_socketEventHandlers) {
        socketEventHandler->stopEventThread();
    }

}


void EventScheduler::setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler){

    for(auto & socketEventHandler : m_socketEventHandlers){
        socketEventHandler->setSocketRemovalHandler(removeSocketEventHandler);
    }
//    for (size_t currentIndex = 0; currentIndex < m_numThreads; currentIndex++) {
//        m_socketEventHandlers.at(currentIndex)->setSocketRemovalHandler(removeSocketEventHandler);
//    }
}

void EventScheduler::setEventDispatcherPtr(EventDispatcher* eventDispatcher){
//    for(auto & )
    for (size_t currentIndex = 0; currentIndex < m_numThreads; currentIndex++) {
        m_socketEventHandlers.at(currentIndex)->setEventDispatcherPtr(eventDispatcher);
    }
}

bool EventScheduler::addSocket(EventStorePointer* eventStorePointer){
    std::unique_lock<std::mutex> lock(m_mutex);
    m_currentIndex = (m_currentIndex + 1) % m_numThreads;
    return m_socketEventHandlers.at(m_currentIndex)->addSocket(eventStorePointer);
}

void EventScheduler::startAllEventHandler(){
    for(size_t currentIndex = 0; currentIndex < m_numThreads; currentIndex++){
        m_socketEventHandlers.at(currentIndex)->startEventReceiverThread();
    }
}
