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

    std::function<void(EventStorePointer*)> taskFunction = m_taskFunction;
    m_threadPool.enqueueTask(eventStorePointer,taskFunction);
}

void IOWorkerThreadHandler::setTaskFunction(const std::function<void(EventStorePointer*)>& taskFunction){
    m_taskFunction = taskFunction;
}

void IOWorkerThreadHandler::handleIOTask(EventStorePointer* eventStorePointer){
    std::string dataReceived;
    int iDataAvailable = eventStorePointer->getAvailableDataInSocket();
    //eventStorePointer->receiveData(dataReceived);
    eventStorePointer->receivePartialData(4,dataReceived);
    std::cout << "iDataAvailable: " << iDataAvailable << ":" << dataReceived << std::endl;
    std::string restDataReceived;
    eventStorePointer->receivePartialData(iDataAvailable - 4,restDataReceived);
    std::cout << "iDataAvailable: " << iDataAvailable << ":" << restDataReceived << std::endl;
}