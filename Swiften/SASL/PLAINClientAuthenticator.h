#pragma once

#include "Swiften/SASL/ClientAuthenticator.h"

namespace Swift {
	class PLAINClientAuthenticator : public ClientAuthenticator {
		public:
			PLAINClientAuthenticator();

			virtual ByteArray getResponse() const;
			virtual bool setChallenge(const ByteArray&);
	};
}
