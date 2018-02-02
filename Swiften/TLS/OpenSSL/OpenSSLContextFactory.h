/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
    class OpenSSLContextFactory : public TLSContextFactory {
        public:
            bool canCreate() const override final;
            virtual TLSContext* createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode mode) override final;

            // Not supported
            virtual void setCheckCertificateRevocation(bool b) override final;
            virtual void setDisconnectOnCardRemoval(bool b) override final;
    };
}
