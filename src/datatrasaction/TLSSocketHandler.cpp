//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#include "TLSSocketHandler.h"


bool TLSSocketHandler::initConnection(EventStorePointer* eventStorePointer){
    return true;
}

ssize_t TLSSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
    return 0;
}

ssize_t TLSSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
    return 0;
}

void TLSSocketHandler::closeConnection(EventStorePointer* eventStorePointer){

}


ssize_t TLSSocketHandler::receivePartialData(EventStorePointer* eventStorePointer,int dataSize, std::string& data){
    return 0;
}