//
// Created by Dell on 7/24/2024.
//

#include "TLSSocketHandler.h"


bool TLSSocketHandler::initConnection(std::unique_ptr<EventStorePointer> eventStorePointer){
    return true;
}

ssize_t TLSSocketHandler::sendData(std::unique_ptr<EventStorePointer> eventStorePointer,const std::string& data){
    return 0;
}

ssize_t TLSSocketHandler::receiveData(std::unique_ptr<EventStorePointer> eventStorePointer, std::string& data){
    return 0;
}

void TLSSocketHandler::closeConnection(){

}