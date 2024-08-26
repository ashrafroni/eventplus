//
// Created by Mohammad Ashraf Hossain on 8/2/2024.
//

#pragma once

#include "../common/CommonDefinition.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <mutex>
#include <memory>
#include <string>

class SSLCertificateHandler
{
public:
    SSLCertificateHandler();
    ~SSLCertificateHandler();

    // Set the certificate and key file names
    void setCertificateAndKeyFileName(const std::string& strCertificateFileName, const std::string& strKeyFileName);

    // Initialize the certification manager
    bool initCertificationManager();

    // Obtain a new SSL certificate
    SSL* getNewCertificate();

    // Display the certificates
    void DisplayCerts(SSL* pSSL);

private:
    // Create the certificate context
    bool createCertificateContext();

    // Configure the SSL context
    void configureSSLContext();

    // Load the certificates from file
    bool loadCertificateFromFile(const std::string& strCertificateFileName, const std::string& strKeyFileName);

    // Load an X509 certificate from file
    std::unique_ptr<X509, decltype(&X509_free)> loadCertificate(const std::string& filename);

    // Load an RSA private key from file
    std::unique_ptr<RSA, decltype(&RSA_free)> loadPrivateKey(const std::string& filename);

private:
    std::string m_strCertificateFileName; // Certificate file name
    std::string m_strKeyFileName; // Key file name
    SSL_CTX *m_pSSLContext; // SSL context
    std::mutex m_mutex; // Mutex for thread safety

    // Smart pointers for OpenSSL objects to ensure proper resource management
    X509 *m_pCertFile; // X509 certificate
    RSA *m_pKeyFile; // RSA private key
};


