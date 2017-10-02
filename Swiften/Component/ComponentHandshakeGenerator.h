/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
    class CryptoProvider;

    class SWIFTEN_API ComponentHandshakeGenerator {
        public:
            static std::string getHandshake(const std::string& streamID, const std::string& secret, CryptoProvider* crypto);
    };

}
