/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// TODO: Get rid of this
//
#pragma once

#include <string>
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PLAINMessage {
		public:
			PLAINMessage(const std::string& authcid, const std::string& password, const std::string& authzid = "");
			PLAINMessage(const ByteArray& value);

			ByteArray getValue() const;

			const std::string& getAuthenticationID() const {
				return authcid;
			}

			const std::string& getPassword() const {
				return password;
			}

			const std::string& getAuthorizationID() const {
				return authzid;
			}

		private:
			std::string authcid;
			std::string authzid;
			std::string password;
	};
}
