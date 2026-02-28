/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class SWIFTEN_API Hash {
        public:
            virtual ~Hash();

            virtual Hash& update(const ByteArray& data) = 0;
            virtual Hash& update(const SafeByteArray& data) = 0;

            virtual std::vector<unsigned char> getHash() = 0;
    };
}
