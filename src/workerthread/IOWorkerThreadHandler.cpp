//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#include "IOWorkerThreadHandler.h"


IOWorkerThreadHandler::IOWorkerThreadHandler(size_t numThreads) : m_threadPool(numThreads){

}
IOWorkerThreadHandler::~IOWorkerThreadHandler(){

}



void IOWorkerThreadHandler::handleEvent(EventStorePointer* eventStorePointer){
    auto boundFunction = std::bind(&IOWorkerThreadHandler::handleIOEvent, this, std::placeholders::_1);
    std::function<void(EventStorePointer*)> taskFunction = boundFunction;
    m_threadPool.enqueueTask(eventStorePointer,taskFunction);
}

void IOWorkerThreadHandler::handleIOEvent(EventStorePointer* eventStorePointer){
    std::string dataReceived;
    int iDataAvailable = eventStorePointer->getAvailableDataInSocket();
    eventStorePointer->receiveData(dataReceived);
    std::cout << "iDataAvailable: " << iDataAvailable << ":" << dataReceived << std::endl;

}