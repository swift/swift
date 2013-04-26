/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */


#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>

#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/Crypto/CryptoProvider.h>

namespace Swift {

IncrementalBytestreamHashCalculator::IncrementalBytestreamHashCalculator(bool doMD5, bool doSHA1, CryptoProvider* crypto) {
	md5Hasher = doMD5 ? crypto->createMD5() : NULL;
	sha1Hasher = doSHA1 ? crypto->createSHA1() : NULL;
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

std::string IncrementalBytestreamHashCalculator::getSHA1String() {
	if (sha1Hasher) {
		ByteArray result = sha1Hasher->getHash();
		return Hexify::hexify(result);
	} else {
		return std::string();
	}
}

std::string IncrementalBytestreamHashCalculator::getMD5String() {
	if (md5Hasher) {
		ByteArray result = md5Hasher->getHash();
		return Hexify::hexify(result);
	} else {
		return std::string();
	}
}

}
