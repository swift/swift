#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class SCRAMSHA1ClientAuthenticator {
		public:
			SCRAMSHA1ClientAuthenticator(const String& authcid, const String& password, const String& authzid, const ByteArray& nonce);

			ByteArray getMessage() const;
			bool setResponse(const ByteArray&);

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
