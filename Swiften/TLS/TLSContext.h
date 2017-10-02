/*
 * Copyright (c) 2010-2016 Isode Limited.
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
#include <Swiften/TLS/TLSError.h>

namespace Swift {

    class SWIFTEN_API TLSContext {
        public:
            virtual ~TLSContext();

            virtual void connect() = 0;

            virtual bool setClientCertificate(CertificateWithKey::ref cert) = 0;

            virtual void handleDataFromNetwork(const SafeByteArray&) = 0;
            virtual void handleDataFromApplication(const SafeByteArray&) = 0;

            Certificate::ref getPeerCertificate() const;
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const = 0;
            virtual CertificateVerificationError::ref getPeerCertificateVerificationError() const = 0;

            virtual ByteArray getFinishMessage() const = 0;

        public:
            boost::signals2::signal<void (const SafeByteArray&)> onDataForNetwork;
            boost::signals2::signal<void (const SafeByteArray&)> onDataForApplication;
            boost::signals2::signal<void (std::shared_ptr<TLSError>)> onError;
            boost::signals2::signal<void ()> onConnected;
    };
}
