/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

class SecureTransportContextFactory : public TLSContextFactory {
    public:
        SecureTransportContextFactory();
        virtual ~SecureTransportContextFactory();

        virtual bool canCreate() const;

        virtual std::unique_ptr<TLSContext> createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode mode = TLSContext::Mode::Client);
        virtual void setCheckCertificateRevocation(bool b);
        virtual void setDisconnectOnCardRemoval(bool b);

    private:
        bool checkCertificateRevocation_;
        bool disconnectOnCardRemoval_;
};

}
