/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Server/UserRegistry.h"

namespace Swift {
	class String;

	class SimpleUserRegistry : public UserRegistry {
		public:
			SimpleUserRegistry();

			virtual bool isValidUserPassword(const JID& user, const String& password) const;
			void addUser(const JID& user, const String& password);

		private:
			std::map<JID, String> users;
	};
}
