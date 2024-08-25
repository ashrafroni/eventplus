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

SSL_CTX* TLSClient::InitCTX() {
    const SSL_METHOD* method = SSLv23_client_method();  // Use
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (ctx == nullptr) {
        ERR_print_errors_fp(stderr);
    }
    return ctx;
}

bool TLSClient::Init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    m_ctx = InitCTX();
    return m_ctx != nullptr;
}

bool TLSClient::CreateSocket() {
    struct addrinfo hints, *res;
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
        m_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (m_socket == -1) {
            continue;
        }
        if (connect(m_socket, p->ai_addr, p->ai_addrlen) == 0) {
            freeaddrinfo(res);
            return true;
        }
        close(m_socket);
        m_socket = -1;
    }

    freeaddrinfo(res);
    return false;
}

bool TLSClient::Connect() {
    if (!CreateSocket()) {
        return false;
    }

    m_ssl = SSL_new(m_ctx);
    if (m_ssl == nullptr) {
        std::cerr << "SSL_new() failed" << std::endl;
        return false;
    }

    SSL_set_fd(m_ssl, m_socket);
    if (SSL_connect(m_ssl) <= 0) {
        SSL_get_error(m_ssl, -1);
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

int TLSClient::SendData(const uint8_t* data, int length) {
    return SSL_write(m_ssl, data, length);
}

std::string TLSClient::ReceiveData(int size) {
    std::string data(size, '\0');
    int bytesRead = SSL_read(m_ssl, &data[0], size);
    if (bytesRead <= 0) {
        data.clear();
    } else {
        data.resize(bytesRead);
    }
    return data;
}

void TLSClient::Close() {
    if (m_ssl) {
        SSL_shutdown(m_ssl);
        SSL_free(m_ssl);
        m_ssl = nullptr;
    }
    if (m_socket != -1) {
        close(m_socket);
        m_socket = -1;
    }
    if (m_ctx) {
        SSL_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
}