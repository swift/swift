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


#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

IncrementalBytestreamHashCalculator::IncrementalBytestreamHashCalculator(bool doMD5, bool doSHA1, CryptoProvider* crypto) {
    md5Hasher = doMD5 ? crypto->createMD5() : nullptr;
    sha1Hasher = doSHA1 ? crypto->createSHA1() : nullptr;
}

IncrementalBytestreamHashCalculator::~IncrementalBytestreamHashCalculator() {
    delete md5Hasher;
    delete sha1Hasher;
}

void IncrementalBytestreamHashCalculator::feedData(const ByteArray& data) {
    if (md5Hasher) {
        md5Hasher->update(data);
    }
    if (sha1Hasher) {
        sha1Hasher->update(data);
    }
}
/*
void IncrementalBytestreamHashCalculator::feedData(const SafeByteArray& data) {
    if (md5Hasher) {
        md5Hasher->update(createByteArray(data.data(), data.size()));
    }
    if (sha1Hasher) {
        sha1Hasher->update(createByteArray(data.data(), data.size()));
    }
}*/

ByteArray IncrementalBytestreamHashCalculator::getSHA1Hash() {
    assert(sha1Hasher);
    if (!sha1Hash) {
        sha1Hash = sha1Hasher->getHash();
    }
    return *sha1Hash;
}

ByteArray IncrementalBytestreamHashCalculator::getMD5Hash() {
    assert(md5Hasher);
    if (!md5Hash) {
        md5Hash = md5Hasher->getHash();
    }
    return *md5Hash;
}

std::string IncrementalBytestreamHashCalculator::getSHA1String() {
    assert(sha1Hasher);
    return Hexify::hexify(getSHA1Hash());;
}

std::string IncrementalBytestreamHashCalculator::getMD5String() {
    assert(md5Hasher);
    return Hexify::hexify(getMD5Hash());;
}

}
