//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#include <cstring>
#include <unistd.h>
#include "NormalSocketHandler.h"
#include <sys/ioctl.h>


NormalSocketHandler::NormalSocketHandler(){

}
NormalSocketHandler::~NormalSocketHandler(){

}
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
    while (true) {
        bytesRead = read(eventStorePointer->m_socketId, buffer, sizeof(buffer));

        if (bytesRead > 0) {
            std::cout << "bytesRead > 0" << buffer << std::endl;
            bufferDataRead.insert(bufferDataRead.end(), buffer, buffer + bytesRead);
        } else if (bytesRead == 0) {
            // Connection closed by the peer
            std::cout << "Socket closed by peer" << std::endl;
            break;
        } else {
            std::cout << "error  " << bytesRead  << std::endl;
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No more data available for now, but connection is still open
                std::cout << "No more data available, will wait for more data" << std::endl;
                break; // or use a mechanism to wait for more data (e.g., epoll, select, etc.)
            } else if (errno == EINTR) {
                // Interrupted by a signal, try again
                continue;
            } else {
                // An error occurred
                std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
                throw std::runtime_error("Socket read error");
            }
        }
    }

    data.assign(bufferDataRead.begin(), bufferDataRead.end());
    return bufferDataRead.size();
}


ssize_t NormalSocketHandler::getAvailableDataInSocket(EventStorePointer* eventStorePointer){
    ssize_t bytesAvailable;
    if (ioctl(eventStorePointer->m_socketId, FIONREAD, &bytesAvailable) == -1) {
        perror("ioctl FIONREAD");
        return -1;

    }
    return bytesAvailable;
}

ssize_t NormalSocketHandler::receivePartialData(EventStorePointer* eventStorePointer,int dataSize, std::string& data){

    return 0;
}
void NormalSocketHandler::closeConnection(EventStorePointer* eventStorePointer){

}