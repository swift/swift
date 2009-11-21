#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class ClientAuthenticator {
		public:
			ClientAuthenticator(const String& name);
			virtual ~ClientAuthenticator();

			const String& getName() const {
				return name;
			}

			void setCredentials(const String& authcid, const String& password, const String& authzid = String()) {
				this->authcid = authcid;
				this->password = password;
				this->authzid = authzid;
			}

			virtual ByteArray getResponse() = 0;
			virtual bool setChallenge(const ByteArray&) = 0;

			const String& getAuthenticationID() const {
				return authcid;
			}

			const String& getAuthorizationID() const {
				return authzid;
			}

			const String& getPassword() const {
				return password;
			}
		
		private:
			String name;
			String authcid;
			String password;
			String authzid;
	};
}
