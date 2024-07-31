//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#pragma once
#include "../common/CommonDefinition.h"
#include "../threadpool/ThreadPool.h"
#include "../datatrasaction/SocketOperationsHandler.h"
#include "../common/EventDispatcher.h"

class IOWorkerThreadHandler : public EventDispatcher {
    IOWorkerThreadHandler(size_t numThreads);
    ~IOWorkerThreadHandler();


    void handleEvent(EventStorePointer* eventStorePointer);

    void handleIOEvent(EventStorePointer* eventStorePointer);

private:
    ThreadPool m_threadPool;
    SocketOperationsHandler* m_socketOperationHandler;
};



