#pragma once

#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/SASL/ClientAuthenticator.h"

namespace Swift {
	class SCRAMSHA1ClientAuthenticator : public ClientAuthenticator {
		public:
			SCRAMSHA1ClientAuthenticator(const String& nonce);
			
			virtual ByteArray getResponse();
			virtual bool setChallenge(const ByteArray&);

		private:
			ByteArray getInitialBareClientMessage() const;
			static std::map<char, String> parseMap(const String&);

		private:
			enum Step {
				Initial,
				Proof
			} step;
			String clientnonce;
			ByteArray initialServerMessage;
			int iterations;
			ByteArray serverNonce;
			ByteArray salt;
			ByteArray serverSignature;
	};
}
