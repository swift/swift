/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

class SecureTransportContextFactory : public TLSContextFactory {
    public:
        SecureTransportContextFactory();
        virtual ~SecureTransportContextFactory();

        virtual bool canCreate() const;

        virtual TLSContext* createTLSContext(const TLSOptions& tlsOptions);
        virtual void setCheckCertificateRevocation(bool b);
        virtual void setDisconnectOnCardRemoval(bool b);

    private:
        bool checkCertificateRevocation_;
        bool disconnectOnCardRemoval_;
};

}
