/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/PrivateKey.h>
#include <Swiften/TLS/TLSError.h>

namespace Swift {

    class SWIFTEN_API TLSContext {
        public:
            virtual ~TLSContext();

            virtual void accept();
            virtual void connect() = 0;
            virtual void connect(const std::string& serverName);

            virtual bool setCertificateChain(const std::vector<Certificate::ref>& /* certificateChain */);
            virtual bool setPrivateKey(const PrivateKey::ref& /* privateKey */);

            virtual bool setClientCertificate(CertificateWithKey::ref cert) = 0;
            virtual bool setDiffieHellmanParameters(const ByteArray& parametersInOpenSslDer);

            /**
             *  This method can be used during the \ref onServerNameRequested signal,
             *  to report an error about an unknown host back to the requesting client.
             */
            virtual void setAbortTLSHandshake(bool /* abort */);

            virtual void handleDataFromNetwork(const SafeByteArray&) = 0;
            virtual void handleDataFromApplication(const SafeByteArray&) = 0;

            Certificate::ref getPeerCertificate() const;
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const = 0;
            virtual CertificateVerificationError::ref getPeerCertificateVerificationError() const = 0;

            virtual ByteArray getFinishMessage() const = 0;
            virtual ByteArray getPeerFinishMessage() const;


        public:
            enum class Mode {
                Client,
                Server
            };

        public:
            boost::signals2::signal<void (const SafeByteArray&)> onDataForNetwork;
            boost::signals2::signal<void (const SafeByteArray&)> onDataForApplication;
            boost::signals2::signal<void (std::shared_ptr<TLSError>)> onError;
            boost::signals2::signal<void ()> onConnected;
            boost::signals2::signal<void (const std::string&)> onServerNameRequested;
    };
}
