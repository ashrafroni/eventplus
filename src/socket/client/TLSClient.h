//
// Created by Dell on 8/24/2024.
//


#pragma once

#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <unistd.h>

class TLSClient {
public:
    TLSClient(const std::string& address, const std::string& port);
    ~TLSClient();

    bool Init();
    bool Connect();
    int SendData(const uint8_t* data, int length);
    std::string ReceiveData(int size);
    void Close();

private:
    SSL_CTX* InitCTX();
    bool CreateSocket();

    std::string m_address;
    std::string m_port;
    SSL_CTX* m_ctx = nullptr;
    SSL* m_ssl = nullptr;
    int m_socket = -1;
};