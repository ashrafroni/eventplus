//
// Created by Dell on 8/26/2024.
//

#include <openssl/err.h>
#include <unistd.h>
#include "TLSClientSocketHandler.h"




void TLSClientSocketHandler::InitCTX() {
    const SSL_METHOD* method = SSLv23_client_method();  // Use
    m_ctx = SSL_CTX_new(method);
    if (m_ctx == nullptr) {
        ERR_print_errors_fp(stderr);
    }

}

bool TLSClientSocketHandler::Init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    InitCTX();
    return m_ctx != nullptr;
}

TLSClientSocketHandler::TLSClientSocketHandler(){
    Init();
}


TLSClientSocketHandler::~TLSClientSocketHandler(){
    if (m_ctx) {
        SSL_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
}


bool TLSClientSocketHandler::initConnection(EventStorePointer* eventStorePointer) {
    if(eventStorePointer == nullptr){
        return false;
    }
    eventStorePointer->m_SSL = SSL_new(m_ctx);

    if (eventStorePointer->m_SSL == nullptr) {
        std::cerr << "SSL_new() failed" << std::endl;
        return false;
    }

    SSL_set_fd(eventStorePointer->m_SSL, eventStorePointer->m_socketId);

    if (SSL_connect(eventStorePointer->m_SSL) <= 0) {
        SSL_get_error(eventStorePointer->m_SSL, -1);
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

ssize_t TLSClientSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
    return 0;
}

ssize_t TLSClientSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
    return 0;
}

ssize_t TLSClientSocketHandler::receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data){
    return 0;
}


ssize_t TLSClientSocketHandler::getAvailableDataInSocket(EventStorePointer* eventStorePointer){
    return 0;
}

void TLSClientSocketHandler::closeConnection(EventStorePointer* eventStorePointer){
    if (eventStorePointer->m_SSL) {
        SSL_shutdown(eventStorePointer->m_SSL);
        SSL_free(eventStorePointer->m_SSL);
        eventStorePointer->m_SSL = nullptr;
    }
    if (eventStorePointer->m_socketId != -1) {
        close(eventStorePointer->m_socketId);
        eventStorePointer->m_socketId = -1;
    }

}