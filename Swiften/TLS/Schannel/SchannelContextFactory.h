/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
    class SchannelContextFactory : public TLSContextFactory {
        public:
            SchannelContextFactory();

            bool canCreate() const;
            virtual std::unique_ptr<TLSContext> createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode mode = TLSContext::Mode::Client);
            virtual void setCheckCertificateRevocation(bool b);

            virtual void setDisconnectOnCardRemoval(bool b);

        public:
            bool checkCertificateRevocation;
            bool disconnectOnCardRemoval;
    };
}
