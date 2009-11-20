#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/SASL/ClientAuthenticator.h"

namespace Swift {
	class SCRAMSHA1ClientAuthenticator : public ClientAuthenticator {
		public:
			SCRAMSHA1ClientAuthenticator(const ByteArray& nonce);
			
			ByteArray getResponse() const;
			bool setChallenge(const ByteArray&);

		private:
			ByteArray getInitialClientMessage() const;
			ByteArray getSalt() const;
			ByteArray getClientVerifier() const;

		private:
			enum Step {
				Initial,
				Proof
			} step;
			String authcid;
			String password;
			String authzid;
			ByteArray clientnonce;
			ByteArray initialServerMessage;
	};
}
