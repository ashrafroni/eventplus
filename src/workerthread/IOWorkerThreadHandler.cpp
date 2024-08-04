//
// Created by Mohammad Ashraf Hossain on 7/31/2024.
//

#include "IOWorkerThreadHandler.h"


IOWorkerThreadHandler::IOWorkerThreadHandler(size_t numThreads) : m_threadPool(numThreads){

}
IOWorkerThreadHandler::~IOWorkerThreadHandler(){

}

void IOWorkerThreadHandler::setSocketOperationHandler(SocketOperationsHandler* socketOperationHandler){
    m_socketOperationHandler = socketOperationHandler;
}

void IOWorkerThreadHandler::handleEvent(EventStorePointer* eventStorePointer){
    auto boundFunction = std::bind(&IOWorkerThreadHandler::handleIOEvent, this, std::placeholders::_1);
    std::function<void(EventStorePointer*)> taskFunction = boundFunction;
    m_threadPool.enqueueTask(eventStorePointer,taskFunction);
}

void IOWorkerThreadHandler::handleIOEvent(EventStorePointer* eventStorePointer){ //SocketOperationsHandler* socketOperationHandler
    std::string dataReceived;
    int iDataAvailable = m_socketOperationHandler->getAvailableDataInSocket(eventStorePointer);
    std::cout << "iDataAvailable:" << iDataAvailable << std::endl;
    m_socketOperationHandler->receiveData(eventStorePointer,dataReceived);
}