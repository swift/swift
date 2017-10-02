/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class SWIFTEN_API Base64 {
        public:
            static std::string encode(const ByteArray& s);
            static SafeByteArray encode(const SafeByteArray& s);

            static ByteArray decode(const std::string &s);
    };
}
