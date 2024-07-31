//
// Created by Mohammad Ashraf Hossain on 7/25/2024.
//


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include "SocketHandler.h"

//void setNonBlocking(int fd) {
//    int flags = fcntl(fd, F_GETFL, 0);
//    if (flags == -1) {
//        perror("fcntl F_GETFL");
//        return;
//    }
//    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
//        perror("fcntl F_SETFL");
//    }
//}

int SocketHandler::createServerSocket(std::string serverIP, std::string serverPort,SocketDetails& socketDetails)
{
    int serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketID < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    socketDetails.m_eventSourceAddress.sin_family = AF_INET;
    socketDetails.m_eventSourceAddress.sin_port = htons(atoi(serverPort.c_str()));
    socketDetails.m_eventSourceAddress.sin_addr.s_addr = inet_addr(serverIP.c_str());
    socketDetails.m_socketId = serverSocketID;

    if (bind(serverSocketID, (struct sockaddr*)& socketDetails.m_eventSourceAddress, sizeof( socketDetails.m_eventSourceAddress)) != 0) {
        close(serverSocketID);
        throw std::runtime_error("Failed to bind socket to port");
    }

    if (listen(serverSocketID, SOMAXCONN) != 0) {
        close(serverSocketID);
        throw std::runtime_error("Failed to listen on socket");
    }

    return serverSocketID;
}

int SocketHandler::createClientSocket(std::string serverIP,std::string strServerPort)
{
    struct addrinfo hints = {0}, *addrs, *addr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(serverIP.c_str(), strServerPort.c_str(), &hints, &addrs);
    if (status != 0) {
        std::cerr << serverIP << ": " << gai_strerror(status) << std::endl;
        return -1;
    }

    int sfd = -1;
    for (addr = addrs; addr != nullptr; addr = addr->ai_next) {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sfd == -1) {
            std::cerr << "socket() error: " << strerror(errno) << std::endl;
            continue;
        }

        if (connect(sfd, addr->ai_addr, addr->ai_addrlen) == 0) {
            break;
        }

        std::cerr << "connect() error: " << strerror(errno) << std::endl;
        close(sfd);
        sfd = -1;
    }

    freeaddrinfo(addrs);

    if (sfd == -1) {
        std::cerr << "Failed to connect to " << serverIP << std::endl;
        return -1;
    }

    return sfd;

}