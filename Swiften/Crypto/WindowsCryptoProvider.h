/*
 * Copyright (c) 2013  Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Base/Override.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class WindowsCryptoProvider : public CryptoProvider, public boost::noncopyable {
		public:
			WindowsCryptoProvider();
			~WindowsCryptoProvider();

			virtual Hash* createSHA1() SWIFTEN_OVERRIDE;
			virtual Hash* createMD5() SWIFTEN_OVERRIDE;
			virtual ByteArray getHMACSHA1(const SafeByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
			virtual ByteArray getHMACSHA1(const ByteArray& key, const ByteArray& data) SWIFTEN_OVERRIDE;
			virtual bool isMD5AllowedForCrypto() const SWIFTEN_OVERRIDE;

		private:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
