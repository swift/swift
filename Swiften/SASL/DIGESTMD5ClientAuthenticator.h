/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/SASL/ClientAuthenticator.h"
#include "Swiften/SASL/DIGESTMD5Properties.h"

namespace Swift {
	class DIGESTMD5ClientAuthenticator : public ClientAuthenticator {
		public:
			DIGESTMD5ClientAuthenticator(const String& host, const String& nonce);
			
			virtual ByteArray getResponse() const;
			virtual bool setChallenge(const ByteArray&);

		private:
			enum Step {
				Initial,
				Response,
				Final,
			} step;
			String host;
			String cnonce;
			DIGESTMD5Properties challenge;
	};
}
