//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#pragma once
#include "../common/CommonDefinition.h"
#include "../threadpool/ThreadPool.h"
#include "../common/BaseSocketHandler.h"
#include "../common/EventDispatcher.h"

class IOWorkerThreadHandler : public EventDispatcher {
public:
    IOWorkerThreadHandler(size_t numThreads);
    ~IOWorkerThreadHandler();


    void handleIOEvent(EventStorePointer* eventStorePointer);
    void setTaskFunction(const std::function<void(EventStorePointer*)>& taskFunction);

private:
    void handleIOTask(EventStorePointer* eventStorePointer); //,BaseSocketHandler* socketOperationHandler


private:
    ThreadPool m_threadPool;
    std::function<void(EventStorePointer*)> m_taskFunction;

};



