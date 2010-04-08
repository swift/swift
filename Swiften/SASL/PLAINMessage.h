/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// TODO: Get rid of this
//
#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PLAINMessage {
		public:
			PLAINMessage(const String& authcid, const String& password, const String& authzid = "");
			PLAINMessage(const ByteArray& value);

			ByteArray getValue() const;

			const String& getAuthenticationID() const {
				return authcid;
			}

			const String& getPassword() const {
				return password;
			}

			const String& getAuthorizationID() const {
				return authzid;
			}

		private:
			String authcid;
			String authzid;
			String password;
	};
}
