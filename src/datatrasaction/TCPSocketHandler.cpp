//
// Created by Mohammad Ashraf Hossain on 7/24/2024.
//

#include <cstring>
#include <unistd.h>
#include "TCPSocketHandler.h"
#include <sys/ioctl.h>


TCPSocketHandler::TCPSocketHandler(){

}
TCPSocketHandler::~TCPSocketHandler(){

}
bool TCPSocketHandler::initConnection(EventStorePointer* eventStorePointer){
    return true;
}

ssize_t TCPSocketHandler::sendData(EventStorePointer* eventStorePointer, std::string& data){
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
            return 0;
        }
        totalSent += bytesSent;
    }

    return totalSent;
}



ssize_t TCPSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
    std::lock_guard<std::mutex> lock(eventStorePointer->m_socketMutex);
    int availableAmount = getAvailableDataInSocket(eventStorePointer);
    return receivePartialData(eventStorePointer,availableAmount,data);
}






ssize_t TCPSocketHandler::getAvailableDataInSocket(EventStorePointer* eventStorePointer){
    ssize_t bytesAvailable;
    if (ioctl(eventStorePointer->m_socketId, FIONREAD, &bytesAvailable) == -1) {
        perror("ioctl FIONREAD");
        return -1;

    }
    return bytesAvailable;
}

ssize_t TCPSocketHandler::receivePartialData(EventStorePointer* eventStorePointer, int dataSize, std::string& data) {
    std::vector<char> buffer(dataSize);
    ssize_t totalBytesRead = 0;

    while (totalBytesRead < dataSize) {
        ssize_t bytesRead = read(eventStorePointer->m_socketId, buffer.data() + totalBytesRead, dataSize - totalBytesRead);

        if (bytesRead < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Socket is non-blocking and no data is available
                break;
            } else {
                // Other read error
                perror("read failed");
                return -1;
            }
        }

        if (bytesRead == 0) {
            // Socket was closed by the peer
            std::cout << "Socket closed by peer" << std::endl;
            break;
        }

        totalBytesRead += bytesRead;
    }

    if (totalBytesRead > 0) {
        // Append the read data to the string
        data.append(buffer.data(), totalBytesRead);
    }

    return totalBytesRead;
}

void TCPSocketHandler::closeConnection(EventStorePointer* eventStorePointer){
    close(eventStorePointer->m_socketId);
}



//ssize_t TCPSocketHandler::receiveData(EventStorePointer* eventStorePointer, std::string& data){
//    std::vector<char> bufferDataRead;
//    std::lock_guard<std::mutex> lock(eventStorePointer->m_socketMutex);
//    char buffer[1024];
//    ssize_t bytesRead;
//    while (true) {
//        std::cout << "Before read  " << eventStorePointer->m_socketId << std::endl;
//        bytesRead = read(eventStorePointer->m_socketId, buffer, sizeof(buffer));
//        std::cout << "after read  " << eventStorePointer->m_socketId  << " " << bytesRead << std::endl;
//        if (bytesRead > 0) {
//            bufferDataRead.insert(bufferDataRead.end(), buffer, buffer + bytesRead);
//        } else if (bytesRead == 0) {
//            break;
//        } else {
//            std::cout << "error  " << bytesRead  << std::endl;
//            std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
//            if (errno == EWOULDBLOCK || errno == EAGAIN) {
//                // No more data available for now, but connection is still open
//                std::cout << "No more data available, will wait for more data" << std::endl;
//                break; // or use a mechanism to wait for more data (e.g., epoll, select, etc.)
//            } else if (errno == EINTR) {
//                // Interrupted by a signal, try again
//                continue;
//            } else {
//                // An error occurred
//                std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
//                throw std::runtime_error("Socket read error");
//            }
//        }
//    }
//
//    data.assign(bufferDataRead.begin(), bufferDataRead.end());
//    return bufferDataRead.size();
//}
