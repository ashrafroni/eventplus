#include "SSLCertificateHandler.h"
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/conf.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <memory>

SSLCertificateHandler::SSLCertificateHandler()
        : m_pCertFile(nullptr), m_pKeyFile(nullptr), m_pSSLContext(nullptr)
{
}

SSLCertificateHandler::~SSLCertificateHandler()
{
    if (m_pSSLContext) {
        SSL_CTX_free(m_pSSLContext);
    }
}

bool SSLCertificateHandler::createCertificateContext()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const SSL_METHOD *method;
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    OPENSSL_load_builtin_modules();

    method = TLS_server_method();  // Use the latest server method

    m_pSSLContext = SSL_CTX_new(method);
    if (!m_pSSLContext)
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

void SSLCertificateHandler::configureSSLContext()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (SSL_CTX_load_verify_locations(m_pSSLContext, m_strCertificateFileName.c_str(), m_strKeyFileName.c_str()) != 1) {
        ERR_print_errors_fp(stderr);
    }

    if (SSL_CTX_use_certificate_file(m_pSSLContext, m_strCertificateFileName.c_str(), SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(m_pSSLContext, m_strKeyFileName.c_str(), SSL_FILETYPE_PEM) <= 0 ||
        !SSL_CTX_check_private_key(m_pSSLContext))
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    SSL_CTX_set_verify(m_pSSLContext, SSL_VERIFY_PEER, nullptr);
    SSL_CTX_set_verify_depth(m_pSSLContext, 4);
}

bool SSLCertificateHandler::initCertificationManager()
{
    if (!createCertificateContext() || !loadCertificateFromFile(m_strCertificateFileName, m_strKeyFileName))
    {
        return false;
    }

    configureSSLContext();
    return true;
}

bool SSLCertificateHandler::loadCertificateFromFile(const std::string& certFile, const std::string& keyFile)
{
    m_pCertFile = loadCertificate(certFile).release(); // Release ownership to raw pointer
    m_pKeyFile = loadPrivateKey(keyFile).release();    // Release ownership to raw pointer

    return m_pCertFile != nullptr && m_pKeyFile != nullptr;
}

std::unique_ptr<X509, decltype(&X509_free)> SSLCertificateHandler::loadCertificate(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        BIO* bio = BIO_new_mem_buf(buffer.data(), static_cast<int>(size));
        std::unique_ptr<X509, decltype(&X509_free)> cert(PEM_read_bio_X509(bio, nullptr, 0, nullptr), X509_free);

        BIO_free(bio);
        return cert;
    }

    return std::unique_ptr<X509, decltype(&X509_free)>(nullptr, X509_free); // Properly return an empty unique_ptr
}

std::unique_ptr<RSA, decltype(&RSA_free)> SSLCertificateHandler::loadPrivateKey(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        BIO* bio = BIO_new_mem_buf(buffer.data(), static_cast<int>(size));
        std::unique_ptr<RSA, decltype(&RSA_free)> key(PEM_read_bio_RSAPrivateKey(bio, nullptr, 0, nullptr), RSA_free);

        BIO_free(bio);
        return key;
    }

    return std::unique_ptr<RSA, decltype(&RSA_free)>(nullptr, RSA_free); // Properly return an empty unique_ptr
}

SSL* SSLCertificateHandler::getNewCertificate()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    SSL* pSSL = SSL_new(m_pSSLContext);
    if (!pSSL)
    {
        ERR_print_errors_fp(stderr);
    }

    return pSSL;
}

void SSLCertificateHandler::setCertificateAndKeyFileName(const std::string& certFile, const std::string& keyFile)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_strCertificateFileName = certFile;
    m_strKeyFileName = keyFile;
}

// Display Certificate
void SSLCertificateHandler::DisplayCerts(SSL* pSSL)
{
    X509* cert = SSL_get_peer_certificate(pSSL);
    if (cert)
    {
        std::cout << "Server certificates:\n";
        char* line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        std::cout << "Subject: " << line << "\n";
        OPENSSL_free(line);

        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        std::cout << "Issuer: " << line << "\n";
        OPENSSL_free(line);

        X509_free(cert);
    }
    else
    {
        std::cout << "Info: No client certificates configured.\n";
    }
}