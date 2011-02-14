/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <string>
#include "Swiften/Base/ByteArray.h"
#include "Swiften/SASL/ClientAuthenticator.h"
#include "Swiften/SASL/DIGESTMD5Properties.h"

namespace Swift {
	class DIGESTMD5ClientAuthenticator : public ClientAuthenticator {
		public:
			DIGESTMD5ClientAuthenticator(const std::string& host, const std::string& nonce);
			
			virtual boost::optional<ByteArray> getResponse() const;
			virtual bool setChallenge(const boost::optional<ByteArray>&);

		private:
			enum Step {
				Initial,
				Response,
				Final,
			} step;
			std::string host;
			std::string cnonce;
			DIGESTMD5Properties challenge;
	};
}
