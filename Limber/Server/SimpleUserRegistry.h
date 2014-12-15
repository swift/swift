/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <Swiften/JID/JID.h>
#include <string>
#include "Limber/Server/UserRegistry.h"

namespace Swift {
	

	class SimpleUserRegistry : public UserRegistry {
		public:
			SimpleUserRegistry();

			virtual bool isValidUserPassword(const JID& user, const SafeByteArray& password) const;
			void addUser(const JID& user, const std::string& password);

		private:
			std::map<JID, SafeByteArray> users;
	};
}
