/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/StreamStack/StreamLayer.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/TLSError.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class TLSContext;
    class TLSContextFactory;

    class SWIFTEN_API TLSLayer : public StreamLayer {
        public:
            TLSLayer(std::unique_ptr<TLSContext> tlsContext);
            virtual ~TLSLayer();

            void connect();
            bool setClientCertificate(CertificateWithKey::ref cert);

            Certificate::ref getPeerCertificate() const;
            std::vector<Certificate::ref> getPeerCertificateChain() const;
            std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;

            void writeData(const SafeByteArray& data);
            void handleDataRead(const SafeByteArray& data);

            TLSContext* getContext() const {
                return context_.get();
            }

        public:
            boost::signals2::signal<void (std::shared_ptr<TLSError>)> onError;
            boost::signals2::signal<void ()> onConnected;

        private:
            std::unique_ptr<TLSContext> context_;
    };
}
