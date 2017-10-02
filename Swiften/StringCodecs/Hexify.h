/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
    class SWIFTEN_API Hexify {
        public:
            static std::string hexify(unsigned char byte);
            static std::string hexify(const ByteArray& data);
            static ByteArray unhexify(const std::string& hexstring);
    };
}
