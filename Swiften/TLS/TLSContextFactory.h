/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class TLSContext;

    class SWIFTEN_API TLSContextFactory {
        public:
            virtual ~TLSContextFactory();

            virtual bool canCreate() const = 0;

            virtual TLSContext* createTLSContext(const TLSOptions& tlsOptions) = 0;
            virtual void setCheckCertificateRevocation(bool b) = 0;
            virtual void setDisconnectOnCardRemoval(bool b) = 0;
    };
}
