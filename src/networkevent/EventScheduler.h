//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//

#pragma once


#include "EventHandlerThread.h"
#include "../common/CommonDefinition.h"

class EventScheduler {

public:
    EventScheduler(size_t numThreads);
    ~EventScheduler();

    bool addSocket(EventStorePointer* eventStorePointer);
    void startAllEventHandler();
    void setEventDispatcherPtr(EventDispatcher* eventDispatcher);

private:
    std::vector<std::unique_ptr<EventHandlerThread>> m_socketEventHandlers;
    int m_currentIndex;
    size_t m_numThreads;
    std::mutex m_mutex;
};



