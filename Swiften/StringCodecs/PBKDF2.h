/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {
    class SWIFTEN_API PBKDF2 {
        public:
            static ByteArray encode(const SafeByteArray& password, const ByteArray& salt, int iterations, CryptoProvider* crypto) {
                ByteArray u = crypto->getHMACSHA1(password, concat(salt, createByteArray("\0\0\0\1", 4)));
                ByteArray result(u);
                int i = 1;
                while (i < iterations) {
                    u = crypto->getHMACSHA1(password, u);
                    for (unsigned int j = 0; j < u.size(); ++j) {
                        result[j] ^= u[j];
                    }
                    ++i;
                }
                return result;
            }
    };
}
