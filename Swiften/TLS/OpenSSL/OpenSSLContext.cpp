/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Platform.h>

#ifdef SWIFTEN_PLATFORM_WINDOWS
#include <windows.h>
#include <wincrypt.h>
#endif

#include <cassert>
#include <memory>
#include <vector>


#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>

#if defined(SWIFTEN_PLATFORM_MACOSX)
#include <Security/Security.h>
#endif

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/TLS/OpenSSL/OpenSSLContext.h>
#include <Swiften/TLS/OpenSSL/OpenSSLCertificate.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/PKCS12Certificate.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wsign-conversion"

namespace Swift {

static const int MAX_FINISHED_SIZE = 4096;
static const int SSL_READ_BUFFERSIZE = 8192;

static void freeX509Stack(STACK_OF(X509)* stack) {
    sk_X509_free(stack);
}

namespace {
    class OpenSSLInitializerFinalizer {
        public:
            OpenSSLInitializerFinalizer() {
                SSL_load_error_strings();
                SSL_library_init();
                OpenSSL_add_all_algorithms();

                // Disable compression
                /*
                STACK_OF(SSL_COMP)* compressionMethods = SSL_COMP_get_compression_methods();
                sk_SSL_COMP_zero(compressionMethods);*/
            }

            ~OpenSSLInitializerFinalizer() {
                EVP_cleanup();
            }

            OpenSSLInitializerFinalizer(const OpenSSLInitializerFinalizer &) = delete;
    };

    std::unique_ptr<SSL_CTX> createSSL_CTX(OpenSSLContext::Mode mode) {
        std::unique_ptr<SSL_CTX> sslCtx;
        switch (mode) {
            case OpenSSLContext::Mode::Client:
                sslCtx = std::unique_ptr<SSL_CTX>(SSL_CTX_new(SSLv23_client_method()));
                break;
            case OpenSSLContext::Mode::Server:
                sslCtx = std::unique_ptr<SSL_CTX>(SSL_CTX_new(SSLv23_server_method()));
                break;
        }
        return sslCtx;
    }

    std::string openSSLInternalErrorToString() {
        auto bio = std::shared_ptr<BIO>(BIO_new(BIO_s_mem()), BIO_free);
        ERR_print_errors(bio.get());
        std::string errorString;
        errorString.resize(BIO_pending(bio.get()));
        BIO_read(bio.get(), (void*)errorString.data(), errorString.size());
        return errorString;
    }
 }

OpenSSLContext::OpenSSLContext(Mode mode) : mode_(mode), state_(State::Start) {
    ensureLibraryInitialized();
    context_ = createSSL_CTX(mode_);
    SSL_CTX_set_options(context_.get(), SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    if (mode_ == Mode::Server) {
#if OPENSSL_VERSION_NUMBER < 0x1010
        // Automatically select highest preference curve used for ECDH temporary keys used during
        // key exchange if possible.
        // Since version 1.1.0, this option is always enabled.
        SSL_CTX_set_ecdh_auto(context_.get(), 1);
#endif

        SSL_CTX_set_tlsext_servername_arg(context_.get(), this);
        SSL_CTX_set_tlsext_servername_callback(context_.get(), OpenSSLContext::handleServerNameCallback);
    }

    // TODO: implement CRL checking
    // TODO: download CRL (HTTP transport)
    // TODO: cache CRL downloads for configurable time period

    // TODO: implement OCSP support
    // TODO: handle OCSP stapling see https://www.rfc-editor.org/rfc/rfc4366.txt
    // Load system certs
#if defined(SWIFTEN_PLATFORM_WINDOWS)
    X509_STORE* store = SSL_CTX_get_cert_store(context_.get());
    HCERTSTORE systemStore = CertOpenSystemStore(0, "ROOT");
    if (systemStore) {
        PCCERT_CONTEXT certContext = NULL;
        while (true) {
            certContext = CertFindCertificateInStore(systemStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_ANY, NULL, certContext);
            if (!certContext) {
                break;
            }
            OpenSSLCertificate cert(createByteArray(certContext->pbCertEncoded, certContext->cbCertEncoded));
            if (store && cert.getInternalX509()) {
                X509_STORE_add_cert(store, cert.getInternalX509().get());
            }
        }
    }
#elif !defined(SWIFTEN_PLATFORM_MACOSX)
    SSL_CTX_set_default_verify_paths(context_.get());
#elif defined(SWIFTEN_PLATFORM_MACOSX) && !defined(SWIFTEN_PLATFORM_IPHONE)
    // On Mac OS X 10.5 (OpenSSL < 0.9.8), OpenSSL does not automatically look in the system store.
    // On Mac OS X 10.6 (OpenSSL >= 0.9.8), OpenSSL *does* look in the system store to determine trust.
    // However, if there is a certificate error, it will always emit the "Invalid CA" error if we didn't add
    // the certificates first. See
    //        http://opensource.apple.com/source/OpenSSL098/OpenSSL098-27/src/crypto/x509/x509_vfy_apple.c
    // to understand why. We therefore add all certs from the system store ourselves.
    X509_STORE* store = SSL_CTX_get_cert_store(context_.get());
    CFArrayRef anchorCertificates;
    if (SecTrustCopyAnchorCertificates(&anchorCertificates) == 0) {
        for (int i = 0; i < CFArrayGetCount(anchorCertificates); ++i) {
            SecCertificateRef cert = reinterpret_cast<SecCertificateRef>(const_cast<void*>(CFArrayGetValueAtIndex(anchorCertificates, i)));
            CSSM_DATA certCSSMData;
            if (SecCertificateGetData(cert, &certCSSMData) != 0 || certCSSMData.Length == 0) {
                continue;
            }
            std::vector<unsigned char> certData;
            certData.resize(certCSSMData.Length);
            memcpy(&certData[0], certCSSMData.Data, certCSSMData.Length);
            OpenSSLCertificate certificate(certData);
            if (store && certificate.getInternalX509()) {
                X509_STORE_add_cert(store, certificate.getInternalX509().get());
            }
        }
        CFRelease(anchorCertificates);
    }
#endif
}

OpenSSLContext::~OpenSSLContext() {
}

void OpenSSLContext::ensureLibraryInitialized() {
    static OpenSSLInitializerFinalizer openSSLInit;
}

void OpenSSLContext::initAndSetBIOs() {
    // Ownership of BIOs is transferred
    readBIO_ = BIO_new(BIO_s_mem());
    writeBIO_ = BIO_new(BIO_s_mem());
    SSL_set_bio(handle_.get(), readBIO_, writeBIO_);
}

void OpenSSLContext::accept() {
    assert(mode_ == Mode::Server);
    handle_ = std::unique_ptr<SSL>(SSL_new(context_.get()));
    if (!handle_) {
        state_ = State::Error;
        onError(std::make_shared<TLSError>());
        return;
    }

    initAndSetBIOs();

    state_ = State::Accepting;
    doAccept();
}

void OpenSSLContext::connect() {
    connect(std::string());
}

void OpenSSLContext::connect(const std::string& requestedServerName) {
    assert(mode_ == Mode::Client);
    handle_ = std::unique_ptr<SSL>(SSL_new(context_.get()));
    if (!handle_) {
        state_ = State::Error;
        onError(std::make_shared<TLSError>());
        return;
    }

    if (!requestedServerName.empty()) {
        if (SSL_set_tlsext_host_name(handle_.get(), const_cast<char*>(requestedServerName.c_str())) != 1) {
            SWIFT_LOG(error) << "Failed on SSL_set_tlsext_host_name()." << std::endl;
        }
    }

    // Ownership of BIOs is transferred to the SSL_CTX instance in handle_.
    initAndSetBIOs();

    state_ = State::Connecting;
    doConnect();
}

void OpenSSLContext::doAccept() {
    auto acceptResult = SSL_accept(handle_.get());
    auto error = SSL_get_error(handle_.get(), acceptResult);
    switch (error) {
        case SSL_ERROR_NONE: {
            state_ = State::Connected;
            //std::cout << x->name << std::endl;
            //const char* comp = SSL_get_current_compression(handle_.get());
            //std::cout << "Compression: " << SSL_COMP_get_name(comp) << std::endl;
            onConnected();
            // The following call is important so the client knowns the handshake is finished.
            sendPendingDataToNetwork();
            break;
        }
        case SSL_ERROR_WANT_READ:
            sendPendingDataToNetwork();
            break;
        case SSL_ERROR_WANT_WRITE:
            sendPendingDataToNetwork();
            break;
        default:
            SWIFT_LOG(warning) << openSSLInternalErrorToString() << std::endl;
            state_ = State::Error;
            onError(std::make_shared<TLSError>());
            sendPendingDataToNetwork();
    }
}

void OpenSSLContext::doConnect() {
    int connectResult = SSL_connect(handle_.get());
    int error = SSL_get_error(handle_.get(), connectResult);
    switch (error) {
        case SSL_ERROR_NONE: {
            state_ = State::Connected;
            //std::cout << x->name << std::endl;
            //const char* comp = SSL_get_current_compression(handle_.get());
            //std::cout << "Compression: " << SSL_COMP_get_name(comp) << std::endl;
            onConnected();
            break;
        }
        case SSL_ERROR_WANT_READ:
            sendPendingDataToNetwork();
            break;
        default:
            SWIFT_LOG(warning) << openSSLInternalErrorToString() << std::endl;
            state_ = State::Error;
            onError(std::make_shared<TLSError>());
    }
}

int OpenSSLContext::handleServerNameCallback(SSL* ssl, int*, void* arg) {
    if (ssl == nullptr)
        return SSL_TLSEXT_ERR_NOACK;

    const char* servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    if (servername) {
        auto serverNameString = std::string(servername);
        auto context = reinterpret_cast<OpenSSLContext*>(arg);
        context->onServerNameRequested(serverNameString);

        if (context->abortTLSHandshake_) {
            context->abortTLSHandshake_ = false;
            return SSL_TLSEXT_ERR_ALERT_FATAL;
        }
    }
    return SSL_TLSEXT_ERR_OK;
}

void OpenSSLContext::sendPendingDataToNetwork() {
    int size = BIO_pending(writeBIO_);
    if (size > 0) {
        SafeByteArray data;
        data.resize(size);
        BIO_read(writeBIO_, vecptr(data), size);
        onDataForNetwork(data);
    }
}

void OpenSSLContext::handleDataFromNetwork(const SafeByteArray& data) {
    BIO_write(readBIO_, vecptr(data), data.size());
    switch (state_) {
        case State::Accepting:
            doAccept();
            break;
        case State::Connecting:
            doConnect();
            break;
        case State::Connected:
            sendPendingDataToApplication();
            break;
        case State::Start: assert(false); break;
        case State::Error: /*assert(false);*/ break;
    }
}

void OpenSSLContext::handleDataFromApplication(const SafeByteArray& data) {
    if (SSL_write(handle_.get(), vecptr(data), data.size()) >= 0) {
        sendPendingDataToNetwork();
    }
    else {
        state_ = State::Error;
        onError(std::make_shared<TLSError>());
    }
}

void OpenSSLContext::sendPendingDataToApplication() {
    SafeByteArray data;
    data.resize(SSL_READ_BUFFERSIZE);
    int ret = SSL_read(handle_.get(), vecptr(data), data.size());
    while (ret > 0) {
        data.resize(ret);
        onDataForApplication(data);
        data.resize(SSL_READ_BUFFERSIZE);
        ret = SSL_read(handle_.get(), vecptr(data), data.size());
    }
    if (ret < 0 && SSL_get_error(handle_.get(), ret) != SSL_ERROR_WANT_READ) {
        state_ = State::Error;
        onError(std::make_shared<TLSError>());
    }
}

bool OpenSSLContext::setCertificateChain(const std::vector<Certificate::ref>& certificateChain) {
    if (certificateChain.size() == 0) {
        SWIFT_LOG(warning) << "Trying to load empty certificate chain." << std::endl;
        return false;
    }

    // load endpoint certificate
    auto openSSLCert = std::dynamic_pointer_cast<OpenSSLCertificate>(certificateChain[0]);
    if (!openSSLCert) {
        return false;
    }

    if (SSL_CTX_use_certificate(context_.get(), openSSLCert->getInternalX509().get()) != 1) {
        return false;
    }

    if (certificateChain.size() > 1) {
        for (auto certificate : range(certificateChain.begin() + 1, certificateChain.end())) {
            auto openSSLCert = std::dynamic_pointer_cast<OpenSSLCertificate>(certificate);
            if (!openSSLCert) {
                return false;
            }
            if (SSL_CTX_add_extra_chain_cert(context_.get(), openSSLCert->getInternalX509().get()) != 1) {
                SWIFT_LOG(warning) << "Trying to load empty certificate chain." << std::endl;
                return false;
            }
        }
    }

    if (handle_) {
        // This workaround is needed as OpenSSL has a shortcut to not do anything
        // if you set the SSL_CTX to the existing SSL_CTX and not reloading the
        // certificates from the SSL_CTX.
        auto dummyContext = createSSL_CTX(mode_);
        SSL_set_SSL_CTX(handle_.get(), dummyContext.get());
        SSL_set_SSL_CTX(handle_.get(), context_.get());
    }

    return true;
}

int empty_or_preset_password_cb(char* buf, int max_len, int flag, void* password);

int empty_or_preset_password_cb(char* buf, int max_len, int /* flag */, void* password) {
    char* charPassword = (char*)password;
    if (charPassword == nullptr) {
        return 0;
    }
    int len = strlen(charPassword);
    if(len > max_len) {
        return 0;
    }
    memcpy(buf, charPassword, len);
    return len;
}

bool OpenSSLContext::setPrivateKey(const PrivateKey::ref& privateKey) {
    if (privateKey->getData().size() > std::numeric_limits<int>::max()) {
        return false;
    }

    auto bio = std::shared_ptr<BIO>(BIO_new(BIO_s_mem()), BIO_free);
    BIO_write(bio.get(), vecptr(privateKey->getData()), int(privateKey->getData().size()));

    SafeByteArray safePassword;
    void* password = nullptr;
    if (privateKey->getPassword()) {
        safePassword = privateKey->getPassword().get();
        safePassword.push_back(0);
        password = safePassword.data();
    }
    auto resultKey = PEM_read_bio_PrivateKey(bio.get(), nullptr, empty_or_preset_password_cb, password);
    if (resultKey) {
        if (handle_) {
            auto result = SSL_use_PrivateKey(handle_.get(), resultKey);;
            if (result != 1) {
                return false;
            }
        }
        else {
            auto result = SSL_CTX_use_PrivateKey(context_.get(), resultKey);
            if (result != 1) {
                return false;
            }
        }
    }
    else {
        return false;
    }
    return true;
}

void OpenSSLContext::setAbortTLSHandshake(bool abort) {
    abortTLSHandshake_ = abort;
}

bool OpenSSLContext::setClientCertificate(CertificateWithKey::ref certificate) {
    std::shared_ptr<PKCS12Certificate> pkcs12Certificate = std::dynamic_pointer_cast<PKCS12Certificate>(certificate);
    if (!pkcs12Certificate || pkcs12Certificate->isNull()) {
        return false;
    }

    // Create a PKCS12 structure
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, vecptr(pkcs12Certificate->getData()), pkcs12Certificate->getData().size());
    std::shared_ptr<PKCS12> pkcs12(d2i_PKCS12_bio(bio, nullptr), PKCS12_free);
    BIO_free(bio);
    if (!pkcs12) {
        return false;
    }

    // Parse PKCS12
    X509 *certPtr = nullptr;
    EVP_PKEY* privateKeyPtr = nullptr;
    STACK_OF(X509)* caCertsPtr = nullptr;
    SafeByteArray password(pkcs12Certificate->getPassword());
    password.push_back(0);
    int result = PKCS12_parse(pkcs12.get(), reinterpret_cast<const char*>(vecptr(password)), &privateKeyPtr, &certPtr, &caCertsPtr);
    if (result != 1) {
        return false;
    }
    std::shared_ptr<X509> cert(certPtr, X509_free);
    std::shared_ptr<EVP_PKEY> privateKey(privateKeyPtr, EVP_PKEY_free);
    std::shared_ptr<STACK_OF(X509)> caCerts(caCertsPtr, freeX509Stack);

    // Use the key & certificates
    if (SSL_CTX_use_certificate(context_.get(), cert.get()) != 1) {
        return false;
    }
    if (SSL_CTX_use_PrivateKey(context_.get(), privateKey.get()) != 1) {
        return false;
    }
    for (int i = 0;  i < sk_X509_num(caCerts.get()); ++i) {
        SSL_CTX_add_extra_chain_cert(context_.get(), sk_X509_value(caCerts.get(), i));
    }
    return true;
}

bool OpenSSLContext::setDiffieHellmanParameters(const ByteArray& parametersInOpenSslDer) {
    auto bio = std::unique_ptr<BIO, decltype(&BIO_free)>(BIO_new(BIO_s_mem()), BIO_free);
    if (bio) {
        BIO_write(bio.get(), vecptr(parametersInOpenSslDer), parametersInOpenSslDer.size());
        auto result = 0L;
        if (auto dhparams = d2i_DHparams_bio(bio.get(), NULL)) {
            if (handle_) {
                result = SSL_set_tmp_dh(handle_.get(), dhparams);
            }
            else {
                result = SSL_CTX_set_tmp_dh(context_.get(), dhparams);
            }
            DH_free(dhparams);
        }
        return result == 1;
    }
    return false;
}

std::vector<Certificate::ref> OpenSSLContext::getPeerCertificateChain() const {
    std::vector<Certificate::ref> result;
    STACK_OF(X509)* chain = SSL_get_peer_cert_chain(handle_.get());
    for (int i = 0; i < sk_X509_num(chain); ++i) {
        std::shared_ptr<X509> x509Cert(X509_dup(sk_X509_value(chain, i)), X509_free);

        Certificate::ref cert = std::make_shared<OpenSSLCertificate>(x509Cert);
        result.push_back(cert);
    }
    return result;
}

std::shared_ptr<CertificateVerificationError> OpenSSLContext::getPeerCertificateVerificationError() const {
    int verifyResult = SSL_get_verify_result(handle_.get());
    if (verifyResult != X509_V_OK) {
        return std::make_shared<CertificateVerificationError>(getVerificationErrorTypeForResult(verifyResult));
    }
    else {
        return std::shared_ptr<CertificateVerificationError>();
    }
}

ByteArray OpenSSLContext::getFinishMessage() const {
    ByteArray data;
    data.resize(MAX_FINISHED_SIZE);
    auto size = SSL_get_finished(handle_.get(), vecptr(data), data.size());
    data.resize(size);
    return data;
}

ByteArray OpenSSLContext::getPeerFinishMessage() const {
    ByteArray data;
    data.resize(MAX_FINISHED_SIZE);
    auto size = SSL_get_peer_finished(handle_.get(), vecptr(data), data.size());
    data.resize(size);
    return data;
 }

CertificateVerificationError::Type OpenSSLContext::getVerificationErrorTypeForResult(int result) {
    assert(result != 0);
    switch (result) {
        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
            return CertificateVerificationError::NotYetValid;

        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
            return CertificateVerificationError::Expired;

        case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
        case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            return CertificateVerificationError::SelfSigned;

        case X509_V_ERR_CERT_UNTRUSTED:
            return CertificateVerificationError::Untrusted;

        case X509_V_ERR_CERT_REJECTED:
            return CertificateVerificationError::Rejected;

        case X509_V_ERR_INVALID_PURPOSE:
            return CertificateVerificationError::InvalidPurpose;

        case X509_V_ERR_PATH_LENGTH_EXCEEDED:
            return CertificateVerificationError::PathLengthExceeded;

        case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
        case X509_V_ERR_CERT_SIGNATURE_FAILURE:
        case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
            return CertificateVerificationError::InvalidSignature;

        case X509_V_ERR_INVALID_CA:
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
            return CertificateVerificationError::InvalidCA;

        case X509_V_ERR_SUBJECT_ISSUER_MISMATCH:
        case X509_V_ERR_AKID_SKID_MISMATCH:
        case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH:
        case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
            return CertificateVerificationError::UnknownError;

        // Unused / should not happen
        case X509_V_ERR_CERT_REVOKED:
        case X509_V_ERR_OUT_OF_MEM:
        case X509_V_ERR_UNABLE_TO_GET_CRL:
        case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
        case X509_V_ERR_CRL_SIGNATURE_FAILURE:
        case X509_V_ERR_CRL_NOT_YET_VALID:
        case X509_V_ERR_CRL_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD:
        case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD:
        case X509_V_ERR_CERT_CHAIN_TOO_LONG:
        case X509_V_ERR_APPLICATION_VERIFICATION:
        default:
            return CertificateVerificationError::UnknownError;
    }
}

}
