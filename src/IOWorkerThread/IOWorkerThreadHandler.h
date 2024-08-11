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


    void handleIOEvent(EventStorePointer* eventStorePointer);
    void setTaskFunction(const std::function<void(EventStorePointer*)>& taskFunction);

private:
    void handleIOTask(EventStorePointer* eventStorePointer); //,SocketOperationsHandler* socketOperationHandler


private:
    ThreadPool m_threadPool;
    std::function<void(EventStorePointer*)> m_taskFunction;

};



