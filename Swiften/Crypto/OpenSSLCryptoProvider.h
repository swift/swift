/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Base/Override.h>

namespace Swift {
	class OpenSSLCryptoProvider : public CryptoProvider {
		public:
			OpenSSLCryptoProvider();
			~OpenSSLCryptoProvider();

			virtual Hash* createSHA1() SWIFTEN_OVERRIDE;
			virtual Hash* createMD5() SWIFTEN_OVERRIDE;
			virtual ByteArray getHMACSHA1(const SafeByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
			virtual ByteArray getHMACSHA1(const ByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
			virtual bool isMD5AllowedForCrypto() const SWIFTEN_OVERRIDE;
	};
}
