/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class Hash;
    class CryptoProvider;

    class IncrementalBytestreamHashCalculator {
    public:
        IncrementalBytestreamHashCalculator(bool doMD5, bool doSHA1, CryptoProvider* crypto);
        ~IncrementalBytestreamHashCalculator();

        void feedData(const ByteArray& data);
        //void feedData(const SafeByteArray& data);

        ByteArray getSHA1Hash();
        ByteArray getMD5Hash();

        std::string getSHA1String();
        std::string getMD5String();

    private:
        Hash* md5Hasher;
        Hash* sha1Hasher;
        boost::optional<ByteArray> md5Hash;
        boost::optional<ByteArray> sha1Hash;
    };

}
