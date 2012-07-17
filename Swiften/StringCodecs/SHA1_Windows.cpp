/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


//http://msdn.microsoft.com/en-us/library/aa379908.aspx

#include <Swiften/StringCodecs/SHA1_Windows.h>

namespace {
	HCRYPTPROV context = 0;

	struct ContextDeleter {
		~ContextDeleter() {
			if (context) {
				CryptReleaseContext(context, 0);
				context = 0;
			}
		}
	} contextDeleter;
}

namespace Swift {

SHA1::SHA1() : hash(NULL) {
	if (!context) {
		if (!CryptAcquireContext(&context, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
//		DWORD error = GetLastError();
//		switch (error) {
//			std::cerr << (long)error << std::endl;
//		}
//		assert(false);
			context = 0;
		}
	}

	if (!CryptCreateHash(context, CALG_SHA1, 0, 0, &hash)) {
		hash = NULL;
	}
}

SHA1::~SHA1() {
	if (hash) {
	   CryptDestroyHash(hash);
	}
}

SHA1& SHA1::update(const std::vector<unsigned char>& data) {
	return update(vecptr(data), data.size());
}


SHA1& SHA1::update(const unsigned char* data, size_t dataSize) {
	if (!hash || !context) {
		return *this;
	}
	if (!CryptHashData(hash, const_cast<BYTE*>(data), dataSize, 0)) {
//		DWORD error = GetLastError();
//		switch (error) {
//			std::cerr << (long)error << std::endl;
//		}
//		assert(false);
//	}
	}
	return *this;
}

std::vector<unsigned char> SHA1::getHash() const {
	if (!hash || !context) {
		return std::vector<unsigned char>();
	}
	std::vector<unsigned char> result;
	DWORD hashLength = sizeof(DWORD);
	DWORD hashSize;
	CryptGetHashParam(hash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), &hashLength, 0);
	result.resize(static_cast<size_t>(hashSize));
	if (!CryptGetHashParam(hash, HP_HASHVAL, vecptr(result), &hashSize, 0)) {
//		DWORD error = GetLastError();
//		switch (error) {
//			std::cerr << (long)error << std::endl;
//		}
//		assert(false);
		return std::vector<unsigned char>();
	}
	result.resize(static_cast<size_t>(hashSize));
	return result;
}


ByteArray SHA1::getHash(const ByteArray& data) {
	SHA1 hash;
	hash.update(vecptr(data), data.size());
	return hash.getHash();
}

ByteArray SHA1::getHash(const SafeByteArray& data) {
	SHA1 hash;
	hash.update(vecptr(data), data.size());
	return hash.getHash();
}

}
