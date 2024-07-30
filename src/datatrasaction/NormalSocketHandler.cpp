//
// Created by Dell on 7/24/2024.
//

#include "NormalSocketHandler.h"


bool NormalSocketHandler::initConnection(std::unique_ptr<EventStorePointer> eventStorePointer){
    return true;
}

ssize_t NormalSocketHandler::sendData(std::unique_ptr<EventStorePointer> eventStorePointer,const std::string& data){
    return 0;
}

ssize_t NormalSocketHandler::receiveData(std::unique_ptr<EventStorePointer> eventStorePointer, std::string& data){
    return 0;
}

void NormalSocketHandler::closeConnection(){

}