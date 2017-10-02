/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class TLSContextFactory;
    class CertificateFactory;

    class SWIFTEN_API PlatformTLSFactories {
        public:
            PlatformTLSFactories();
            ~PlatformTLSFactories();

            TLSContextFactory* getTLSContextFactory() const;
            CertificateFactory* getCertificateFactory() const;

        private:
            TLSContextFactory* contextFactory;
            CertificateFactory* certificateFactory;
    };
}
