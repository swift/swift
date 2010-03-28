#pragma once

#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/SASL/ClientAuthenticator.h"

namespace Swift {
	class SCRAMSHA1ClientAuthenticator : public ClientAuthenticator {
		public:
			SCRAMSHA1ClientAuthenticator(const String& nonce);
			
			virtual ByteArray getResponse() const;
			virtual bool setChallenge(const ByteArray&);

		private:
			ByteArray getInitialBareClientMessage() const;
			ByteArray getGS2Header() const;

			static std::map<char, String> parseMap(const String&);

		private:
			enum Step {
				Initial,
				Proof,
        Final
			} step;
			String clientnonce;
			ByteArray initialServerMessage;
			ByteArray serverNonce;
			ByteArray authMessage;
			ByteArray saltedPassword;
			ByteArray serverSignature;
	};
}
