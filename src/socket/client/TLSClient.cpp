//
// Created by Dell on 8/24/2024.
//

#include <cstring>
#include "TLSClient.h"


TLSClient::TLSClient(const std::string& address, const std::string& port)
        : m_address(address), m_port(port) {}

TLSClient::~TLSClient() {
    Close();
}

void TLSClient::InitCTX() {
    const SSL_METHOD* method = SSLv23_client_method();  // Use
    m_ctx = SSL_CTX_new(method);
    if (m_ctx == nullptr) {
        ERR_print_errors_fp(stderr);
    }

}

bool TLSClient::Init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    InitCTX();
    return m_ctx != nullptr;
}

bool TLSClient::CreateSocket() {
    struct addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(m_address.c_str(), m_port.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return false;
    }

    for (struct addrinfo *p = res; p != nullptr; p = p->ai_next) {
        m_socketDetails.m_socketId = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (m_socketDetails.m_socketId == -1) {
            continue;
        }
        if (connect(m_socketDetails.m_socketId, p->ai_addr, p->ai_addrlen) == 0) {
            freeaddrinfo(res);
            return true;
        }
        close(m_socketDetails.m_socketId);
        m_socketDetails.m_socketId = -1;
    }

    freeaddrinfo(res);
    return false;
}


void TLSClient::handleIOEvent(EventStorePointer* eventStorePointer){

}

bool TLSClient::Connect() {
    if (!CreateSocket()) {
        return false;
    }

    m_clientSocket = std::make_unique<EventStorePointer>(m_socketDetails);

    m_clientSocket->m_SSL = SSL_new(m_ctx);
    if (m_clientSocket->m_SSL == nullptr) {
        std::cerr << "SSL_new() failed" << std::endl;
        return false;
    }

    SSL_set_fd(m_clientSocket->m_SSL, m_socketDetails.m_socketId);
    if (SSL_connect(m_clientSocket->m_SSL) <= 0) {
        SSL_get_error(m_clientSocket->m_SSL, -1);
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

int TLSClient::SendData(const uint8_t* data, int length) {
    return SSL_write(m_clientSocket->m_SSL, data, length);
}

std::string TLSClient::ReceiveData(int size) {
    std::string data(size, '\0');
    int bytesRead = SSL_read(m_clientSocket->m_SSL, &data[0], size);
    if (bytesRead <= 0) {
        data.clear();
    } else {
        data.resize(bytesRead);
    }
    return data;
}

void TLSClient::Close() {
    if (m_clientSocket->m_SSL) {
        SSL_shutdown(m_clientSocket->m_SSL);
        SSL_free(m_clientSocket->m_SSL);
        m_clientSocket->m_SSL = nullptr;
    }
    if (m_socketDetails.m_socketId != -1) {
        close(m_socketDetails.m_socketId);
        m_socketDetails.m_socketId = -1;
    }
    if (m_ctx) {
        SSL_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
}