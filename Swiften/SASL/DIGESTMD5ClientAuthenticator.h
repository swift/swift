/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <string>
#include <vector>
#include <Swiften/Base/API.h>
#include <Swiften/SASL/ClientAuthenticator.h>
#include <Swiften/SASL/DIGESTMD5Properties.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class CryptoProvider;

	class SWIFTEN_API DIGESTMD5ClientAuthenticator : public ClientAuthenticator {
		public:
			DIGESTMD5ClientAuthenticator(const std::string& host, const std::string& nonce, CryptoProvider*);
			
			virtual boost::optional<SafeByteArray> getResponse() const;
			virtual bool setChallenge(const boost::optional<std::vector<unsigned char> >&);

		private:
			enum Step {
				Initial,
				Response,
				Final
			} step;
			std::string host;
			std::string cnonce;
			CryptoProvider* crypto;
			DIGESTMD5Properties challenge;
	};
}
