//
// Created by Dell on 8/26/2024.
//

#include <openssl/err.h>
#include <unistd.h>
#include <cstring>
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

void printSSLError(SSL* ssl) {
    int error = SSL_get_error(ssl, -1);
    switch (error) {
        case SSL_ERROR_NONE:
            std::cerr << "SSL_ERROR_NONE: The TLS/SSL I/O operation completed." << std::endl;
            break;
        case SSL_ERROR_ZERO_RETURN:
            std::cerr << "SSL_ERROR_ZERO_RETURN: The TLS/SSL connection has been closed." << std::endl;
            break;
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            std::cerr << "SSL_ERROR_WANT_READ/WRITE: The operation did not complete." << std::endl;
            break;
        case SSL_ERROR_SYSCALL:
            std::cerr << "SSL_ERROR_SYSCALL: Some I/O error occurred." << std::endl;
            perror("SYSCALL error: ");
            break;
        case SSL_ERROR_SSL:
            std::cerr << "SSL_ERROR_SSL: A failure in the SSL library occurred." << std::endl;
            break;
        default:
            std::cerr << "Unknown error occurred in SSL_connect." << std::endl;
            break;
    }
    ERR_print_errors_fp(stderr);
}

bool TLSClientSocketHandler::initConnection(EventStorePointer* eventStorePointer) {
    if(eventStorePointer == nullptr){
        return false;
    }
    std::cout << "TLSClient::initConnection() SSL_new" << std::endl;
    eventStorePointer->m_SSL = SSL_new(m_ctx);

    if (eventStorePointer->m_SSL == nullptr) {
        std::cerr << "SSL_new() failed" << std::endl;
        return false;
    }
    std::cout << "TLSClient::initConnection() SSL_set_fd" << std::endl;
    SSL_set_fd(eventStorePointer->m_SSL, eventStorePointer->m_socketId);
    std::cout << "TLSClient::initConnection() SSL_connect" << std::endl;
    if (SSL_connect(eventStorePointer->m_SSL) <= 0) {
        SSL_get_error(eventStorePointer->m_SSL, -1);
        ERR_print_errors_fp(stderr);
        std::cout << "TLSClient::initConnection() SSL_connect faiol" << std::endl;
        printSSLError(eventStorePointer->m_SSL);
        return false;
    }
    std::cout << "TLSClient::initConnection() SSL_connect ok" << std::endl;
    return true;
}





ssize_t TLSClientSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
    if(eventStorePointer == nullptr){
        return 0;
    }
    return eventStorePointer->sendData(data);
}

ssize_t TLSClientSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
    if (eventStorePointer == nullptr || eventStorePointer->m_SSL == nullptr) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(eventStorePointer->m_socketMutex);
    const int bufferSize = 1000; // Adjust this to your desired buffer size
    std::vector<char> receivedData;
    int totalBytesRead = 0;

    while (true) {
        std::vector<char> buffer(bufferSize);

        int bytesRead = SSL_read(eventStorePointer->m_SSL, buffer.data(), buffer.size());
        if (bytesRead > 0) {
            receivedData.insert(receivedData.end(), buffer.begin(), buffer.begin() + bytesRead);
            totalBytesRead += bytesRead;
        } else {
            int sslError = SSL_get_error(eventStorePointer->m_SSL, bytesRead);

            if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
                // The operation would block, stop for now and retry later after epoll
                break;
            } else if (sslError == SSL_ERROR_ZERO_RETURN) {
                // The connection was closed cleanly
                break;
            } else {
                // Handle other errors
                std::cerr << "SSL_read failed: " << strerror(errno) << std::endl;
                return -1;
            }
        }

        if (bytesRead < bufferSize) {
            break;
        }
    }

    if (totalBytesRead > 0) {
        data.append(receivedData.begin(), receivedData.end());
    }
//    if(totalBytesRead == 0 && m_removeSocketEventHandler != nullptr)
//        m_removeSocketEventHandler->removeSocket(eventStorePointer);
    return totalBytesRead;
}

ssize_t TLSClientSocketHandler::receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data){
    return 0;
}


ssize_t TLSClientSocketHandler::getAvailableDataInSocket(EventStorePointer* eventStorePointer){
    return 0;
}
void TLSClientSocketHandler::closeConnection(EventStorePointer* eventStorePointer) {
    if (eventStorePointer == nullptr) {
        std::cerr << "EventStorePointer is null!" << std::endl;
        return;
    }

    std::cout << "closeConnection in client !" << std::endl;
    if (eventStorePointer->m_SSL) {
        std::cout << "SSL_shutdown !" << std::endl;
        int shutdownStatus = SSL_shutdown(eventStorePointer->m_SSL);
        std::cout << "SSL_shutdown after !" << std::endl;
        if (shutdownStatus == 0) {
            // If the return value is 0, a bidirectional shutdown is required
            shutdownStatus = SSL_shutdown(eventStorePointer->m_SSL);
            if (shutdownStatus < 0) {
                // Handle SSL shutdown error after the second call
                int sslError = SSL_get_error(eventStorePointer->m_SSL, shutdownStatus);
                std::cerr << "SSL shutdown error: " << sslError << std::endl;
            }
        } else if (shutdownStatus < 0) {
            // Handle SSL shutdown error on the first call
            int sslError = SSL_get_error(eventStorePointer->m_SSL, shutdownStatus);
            std::cerr << "SSL shutdown error: " << sslError << std::endl;
        }

        // Free the SSL structure
        SSL_free(eventStorePointer->m_SSL);
        eventStorePointer->m_SSL = nullptr;
    }

    if (eventStorePointer->m_socketId != -1) {
        // Close the socket
        if (close(eventStorePointer->m_socketId) == -1) {
            // Log socket close error
            std::cerr << "Socket close error: " << errno << std::endl;
        }
        eventStorePointer->m_socketId = -1;
    }
}


void TLSClientSocketHandler::setSocketRemovalHandler(SocketRemovalHandler* removeSocketEventHandler){

}