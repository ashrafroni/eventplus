//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#pragma once
#include "../common/CommonDefinition.h"
#include "../threadpool/ThreadPool.h"
#include "../common/SocketOperationsHandler.h"
#include "../common/EventDispatcher.h"

class IOWorkerThreadHandler : public EventDispatcher {
public:
    IOWorkerThreadHandler(size_t numThreads);
    ~IOWorkerThreadHandler();


    void handleEvent(EventStorePointer* eventStorePointer);

private:
    void handleIOEvent(EventStorePointer* eventStorePointer); //,SocketOperationsHandler* socketOperationHandler


private:
    ThreadPool m_threadPool;

};



