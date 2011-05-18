/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// TODO: Get rid of this
//
#pragma once

#include <string>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class PLAINMessage {
		public:
			PLAINMessage(const std::string& authcid, const SafeByteArray& password, const std::string& authzid = "");
			PLAINMessage(const SafeByteArray& value);

			SafeByteArray getValue() const;

			const std::string& getAuthenticationID() const {
				return authcid;
			}

			const SafeByteArray& getPassword() const {
				return password;
			}

			const std::string& getAuthorizationID() const {
				return authzid;
			}

		private:
			std::string authcid;
			std::string authzid;
			SafeByteArray password;
	};
}
