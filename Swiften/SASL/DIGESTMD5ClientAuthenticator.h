/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/SASL/ClientAuthenticator.h>
#include <Swiften/SASL/DIGESTMD5Properties.h>

namespace Swift {
    class CryptoProvider;

    class SWIFTEN_API DIGESTMD5ClientAuthenticator : public ClientAuthenticator {
        public:
            DIGESTMD5ClientAuthenticator(const std::string& host, const std::string& nonce, CryptoProvider*);

            virtual boost::optional<SafeByteArray> getResponse() const;
            virtual bool setChallenge(const boost::optional<std::vector<unsigned char> >&);

        private:
            enum Step {
                Initial,
                Response,
                Final
            } step;
            std::string host;
            std::string cnonce;
            CryptoProvider* crypto;
            DIGESTMD5Properties challenge;
    };
}
