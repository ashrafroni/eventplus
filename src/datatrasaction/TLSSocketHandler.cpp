//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include "TLSSocketHandler.h"

TLSSocketHandler::TLSSocketHandler(){

}

TLSSocketHandler::~TLSSocketHandler(){

}

bool TLSSocketHandler::initConnection(EventStorePointer* eventStorePointer) {
    // Get new SSL certificate
    eventStorePointer->m_SSL = m_sslCertificateHandler->getNewCertificate();
    if (eventStorePointer->m_SSL == nullptr) {
        std::cerr << "Failed to get new SSL certificate" << std::endl;
        close(eventStorePointer->m_socketId);
        return false;
    }

    // Set the file descriptor
    int iSetFd = SSL_set_fd(eventStorePointer->m_SSL, eventStorePointer->m_socketId);


        int ssl_err = SSL_accept(eventStorePointer->m_SSL);
        if (ssl_err <= 0) {
            int err = SSL_get_error(eventStorePointer->m_SSL, ssl_err);

            std::cerr << "SSL_accept failed with error code: " << err << std::endl;
            switch (err) {
                case SSL_ERROR_NONE:
                    std::cerr << "SSL_ERROR_NONE: No error. Should not reach here." << std::endl;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                    std::cerr << "SSL_ERROR_ZERO_RETURN: TLS/SSL connection has been closed." << std::endl;
                    break;
                case SSL_ERROR_WANT_READ:
                    std::cerr << "SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                case SSL_ERROR_WANT_CONNECT:
                    std::cerr << "SSL_ERROR_WANT_CONNECT" << std::endl;
                    break;
                case SSL_ERROR_WANT_ACCEPT:
                    std::cerr << "SSL_ERROR_WANT_ACCEPT" << std::endl;
                    break;
                case SSL_ERROR_WANT_X509_LOOKUP:
                    std::cerr << "SSL_ERROR_WANT_X509_LOOKUP" << std::endl;
                    break;
                case SSL_ERROR_SYSCALL:
                    if (ssl_err == 0) {
                        std::cerr << "SSL_ERROR_SYSCALL: EOF while processing handshake." << std::endl;
                    } else if (ssl_err == -1) {
                        std::cerr << "SSL_ERROR_SYSCALL: I/O error: " << strerror(errno) << std::endl;
                    } else {
                        std::cerr << "SSL_ERROR_SYSCALL: Unknown syscall error." << std::endl;
                    }
                    break;
                case SSL_ERROR_SSL:
                    std::cerr << "SSL_ERROR_SSL: SSL library error: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
                    break;
                default:
                    std::cerr << "SSL_accept failed with unknown error: " << err << std::endl;
                    break;
            }

        close(eventStorePointer->m_socketId);
        SSL_free(eventStorePointer->m_SSL);
        eventStorePointer->m_SSL = nullptr;
        return false;
    }

    std::cout << "SSL connection successfully established!"   << std::endl;
    return true;
}

ssize_t TLSSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
    return SSL_write(eventStorePointer->m_SSL, data.c_str(), data.length());
}

ssize_t TLSSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
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

    return totalBytesRead;
}

void TLSSocketHandler::closeConnection(EventStorePointer* eventStorePointer) {
    if (eventStorePointer == nullptr || eventStorePointer->m_SSL == nullptr) {
        return;
    }

    SSL* pSSL = eventStorePointer->m_SSL;
    int iSocketID = eventStorePointer->m_socketId;

    // Perform SSL shutdown
    int iSSLShutdown = SSL_shutdown(pSSL);

    if (iSSLShutdown < 0) {
        int sslError = SSL_get_error(pSSL, iSSLShutdown);
        std::cout << "Remove Socket Failed: " << iSocketID << " SSLShutdown error: " << sslError << std::endl;
    }

    // Close the socket
    int iCloseStatus = close(iSocketID);
    if (iCloseStatus < 0) {
        perror("Error closing socket");
    }

    // Free the SSL structure
    SSL_free(pSSL);
    eventStorePointer->m_SSL = nullptr;

    std::cout << "Closed Socket: " << iSocketID << " SSLShutdown result: " << iSSLShutdown << " CloseStatus: " << iCloseStatus << std::endl;
}


ssize_t TLSSocketHandler::receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data) {
    std::vector<char> receivedData;
    int totalBytesRead = 0;
    int remainingSize = dataSize; // Keep track of how much more data we need to read

    while (remainingSize > 0) {
        std::vector<char> buffer(std::min(remainingSize, 1000)); // Adjust buffer size to the smaller of remainingSize or bufferSize

        int bytesRead = SSL_read(eventStorePointer->m_SSL, buffer.data(), buffer.size());
        if (bytesRead > 0) {
            receivedData.insert(receivedData.end(), buffer.begin(), buffer.begin() + bytesRead);
            totalBytesRead += bytesRead;
            remainingSize -= bytesRead; // Decrease remaining size by the number of bytes read
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

        if (bytesRead < buffer.size()) {
            // No more data available or the connection was closed
            break;
        }
    }

    if (totalBytesRead > 0) {
        data.append(receivedData.begin(), receivedData.end());
    }

    return totalBytesRead;
}




ssize_t TLSSocketHandler::getAvailableDataInSocket(EventStorePointer* eventStorePointer){
    //TODO:this function is not implemented
    return -1;
}


bool TLSSocketHandler::loadCertificate(const std::string& certificateFileName, const std::string& keyFileName){
    m_sslCertificateHandler = new SSLCertificateHandler();
    m_sslCertificateHandler->setCertificateAndKeyFileName(certificateFileName, keyFileName);
    return m_sslCertificateHandler->initCertificationManager();
}