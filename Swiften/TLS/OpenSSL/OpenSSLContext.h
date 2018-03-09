/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

#include <openssl/ssl.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/TLSContext.h>

namespace std {
    template<>
    class default_delete<SSL_CTX> {
    public:
        void operator()(SSL_CTX *ptr) {
            SSL_CTX_free(ptr);
        }
    };

    template<>
    class default_delete<SSL> {
    public:
        void operator()(SSL *ptr) {
            SSL_free(ptr);
        }
    };
}

namespace Swift {
    class OpenSSLContext : public TLSContext, boost::noncopyable {
        public:
            OpenSSLContext(Mode mode);
            virtual ~OpenSSLContext() override final;

            void accept() override final;
            void connect() override final;
            void connect(const std::string& requestHostname) override final;

            bool setCertificateChain(const std::vector<Certificate::ref>& certificateChain) override final;
            bool setPrivateKey(const PrivateKey::ref& privateKey) override final;
            bool setClientCertificate(CertificateWithKey::ref cert) override final;
            void setAbortTLSHandshake(bool abort) override final;
            bool setDiffieHellmanParameters(const ByteArray& parametersInOpenSslDer) override final;

            void handleDataFromNetwork(const SafeByteArray&) override final;
            void handleDataFromApplication(const SafeByteArray&) override final;

            std::vector<Certificate::ref> getPeerCertificateChain() const override final;
            std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const override final;

            virtual ByteArray getFinishMessage() const override final;
            virtual ByteArray getPeerFinishMessage() const override final;

        private:
            static void ensureLibraryInitialized();
            static int handleServerNameCallback(SSL *ssl, int *ad, void *arg);
            static CertificateVerificationError::Type getVerificationErrorTypeForResult(int);

            void initAndSetBIOs();
            void doAccept();
            void doConnect();
            void sendPendingDataToNetwork();
            void sendPendingDataToApplication();

        private:
            enum class State { Start, Accepting, Connecting, Connected, Error };

            const Mode mode_;
            State state_;
            std::unique_ptr<SSL_CTX> context_;
            std::unique_ptr<SSL> handle_;
            BIO* readBIO_ = nullptr;
            BIO* writeBIO_ = nullptr;
            bool abortTLSHandshake_ = false;
    };
}
