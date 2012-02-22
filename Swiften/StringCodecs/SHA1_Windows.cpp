/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


//http://msdn.microsoft.com/en-us/library/aa379908.aspx

#include <Swiften/StringCodecs/SHA1_Windows.h>

namespace Swift {

SHA1::SHA1() : hCryptProv(NULL), hHash(NULL) {
	bool hasContext = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	if (!hasContext) {
//		DWORD error = GetLastError();
//		switch (error) {
//			std::cerr << (long)error << std::endl;
//		}
//		assert(false);
		hCryptProv = NULL;
	}

	if (!CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hHash)) {
		hHash = NULL;
	}
}

SHA1::~SHA1() {
	if(hHash) {
	   CryptDestroyHash(hHash);
	}
	if(hCryptProv) {
	   CryptReleaseContext(hCryptProv,0);
	}

}

SHA1& SHA1::update(const std::vector<unsigned char>& data) {
	return update(vecptr(data), data.size());
}


SHA1& SHA1::update(const unsigned char* data, size_t dataSize) {
	if (!hHash || !hCryptProv) {
		return *this;
	}
	BYTE* byteData = (BYTE *)data;
	DWORD dataLength = dataSize;
	bool hasHashed = CryptHashData(hHash, byteData, dataLength, 0);
//	if (!hasHashed) {
//		DWORD error = GetLastError();
//		switch (error) {
//			std::cerr << (long)error << std::endl;
//		}
//		assert(false);
//	}
	return *this;
}

std::vector<unsigned char> SHA1::getHash() const {
	if (!hHash || !hCryptProv) {
		return std::vector<unsigned char>();
	}
	std::vector<unsigned char> result;
	DWORD hashLength = sizeof(DWORD);
	DWORD hashSize;
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashSize, &hashLength, 0);
	result.resize(static_cast<size_t>(hashSize));
	bool hasHashed = CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)vecptr(result), &hashSize, 0);
	if (!hasHashed) {
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
