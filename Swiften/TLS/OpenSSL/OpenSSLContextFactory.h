/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
    class OpenSSLContextFactory : public TLSContextFactory {
        public:
            bool canCreate() const override final;
            virtual std::unique_ptr<TLSContext> createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode mode) override final;

            virtual ByteArray convertDHParametersFromPEMToDER(const std::string& dhParametersInPEM) override final;

            // Not supported
            virtual void setCheckCertificateRevocation(bool b) override final;
            virtual void setDisconnectOnCardRemoval(bool b) override final;
    };
}
