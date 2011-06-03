/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <string>
#include <vector>

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class ClientAuthenticator {
		public:
			ClientAuthenticator(const std::string& name);
			virtual ~ClientAuthenticator();

			const std::string& getName() const {
				return name;
			}

			void setCredentials(const std::string& authcid, const SafeByteArray& password, const std::string& authzid = std::string()) {
				this->authcid = authcid;
				this->password = password;
				this->authzid = authzid;
			}

			virtual boost::optional<SafeByteArray> getResponse() const = 0;
			virtual bool setChallenge(const boost::optional<ByteArray>&) = 0;

			const std::string& getAuthenticationID() const {
				return authcid;
			}

			const std::string& getAuthorizationID() const {
				return authzid;
			}

			const SafeByteArray& getPassword() const {
				return password;
			}
		
		private:
			std::string name;
			std::string authcid;
			SafeByteArray password;
			std::string authzid;
	};
}
