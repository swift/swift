/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {

class MD5;
class SHA1;

class IncrementalBytestreamHashCalculator {
public:
	IncrementalBytestreamHashCalculator(bool doMD5, bool doSHA1);
	~IncrementalBytestreamHashCalculator();

	void feedData(const ByteArray& data);
	//void feedData(const SafeByteArray& data);

	std::string getSHA1String();
	std::string getMD5String();

private:
	MD5* md5Hasher;
	SHA1* sha1Hasher;
};

}
