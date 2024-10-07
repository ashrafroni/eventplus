//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#include "IOWorkerThreadHandler.h"


IOWorkerThreadHandler::IOWorkerThreadHandler(size_t numThreads) : m_threadPool(numThreads){
    m_taskFunction = std::bind(&IOWorkerThreadHandler::handleIOEvent, this, std::placeholders::_1);
}

IOWorkerThreadHandler::~IOWorkerThreadHandler(){

}

void IOWorkerThreadHandler::handleIOEvent(EventStorePointer* eventStorePointer){

    m_threadPool.enqueueTask(eventStorePointer,m_taskFunction);
}

void IOWorkerThreadHandler::setTaskFunction(const std::function<void(EventStorePointer*)>& taskFunction){
    m_taskFunction = taskFunction;
}

void IOWorkerThreadHandler::handleIOTask(EventStorePointer* eventStorePointer){
    std::string dataReceived;
    int iDataAvailable = eventStorePointer->getAvailableDataInSocket();
    eventStorePointer->receivePartialData(4,dataReceived);

    std::string restDataReceived;
    eventStorePointer->receivePartialData(iDataAvailable - 4,restDataReceived);

}