//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#include <cstring>
#include <unistd.h>
#include "NormalSocketHandler.h"


bool NormalSocketHandler::initConnection(EventStorePointer* eventStorePointer){
    return true;
}

ssize_t NormalSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
    ssize_t totalSent = 0;

    std::lock_guard<std::mutex> lock(eventStorePointer->m_socketMutex);

    size_t dataLength = data.length();
    const char* dataPtr = data.c_str();

    while (totalSent < dataLength) {
        ssize_t bytesSent = send(eventStorePointer->m_socketId, dataPtr + totalSent, dataLength - totalSent, 0);

        if (bytesSent < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("send failed");
                return -1;
            }
        }

        if (bytesSent == 0) {
            // Connection closed by the peer
            return 0;
        }

        totalSent += bytesSent;
    }

    return totalSent;
}

ssize_t NormalSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
    std::vector<char> bufferDataRead;
    std::lock_guard<std::mutex> lock(eventStorePointer->m_socketMutex);

    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(eventStorePointer->m_socketId, buffer, sizeof(buffer))) > 0) {
        bufferDataRead.insert(bufferDataRead.end(), buffer, buffer + bytesRead);
    }

    if (bytesRead == -1 && errno != EWOULDBLOCK && errno != EAGAIN) {
        std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
        throw std::runtime_error("Socket read error");
    }

    if (bytesRead == 0) {
        std::cout << "Socket closed by peer" << std::endl;
        return 0;
    }

    data.assign(bufferDataRead.begin(), bufferDataRead.end());
    return bufferDataRead.size();
}

void NormalSocketHandler::closeConnection(){

}