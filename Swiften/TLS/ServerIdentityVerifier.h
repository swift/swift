/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/TLS/Certificate.h>

namespace Swift {
    class IDNConverter;

    class SWIFTEN_API ServerIdentityVerifier {
        public:
            ServerIdentityVerifier(const JID& jid, IDNConverter* idnConverter);

            bool certificateVerifies(Certificate::ref);

        private:
            bool matchesDomain(const std::string&) const ;
            bool matchesAddress(const std::string&) const;

        private:
            std::string domain;
            std::string encodedDomain;
            bool domainValid;
    };
}
