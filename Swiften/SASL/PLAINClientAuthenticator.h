#pragma once

#include "Swiften/SASL/ClientAuthenticator.h"

namespace Swift {
	class PLAINClientAuthenticator : public ClientAuthenticator {
		public:
			PLAINClientAuthenticator();

			virtual ByteArray getResponse();
			virtual bool setChallenge(const ByteArray&);
	};
}
