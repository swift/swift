/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <set>
#include <vector>

#include <string>

namespace Swift {
	class JID;
	class VCardManager;

	class ContactEditWindow {
		public:
			virtual ~ContactEditWindow() {}

			virtual void setEnabled(bool b) = 0;

			virtual void setNameSuggestions(const std::vector<std::string>& suggestions) = 0;
			virtual void setContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, const std::set<std::string>& allGroups) = 0;

			virtual void show() = 0;
			virtual void hide() = 0;

			boost::signal<void ()> onRemoveContactRequest;
			boost::signal<void (const std::string& /* name */, const std::set<std::string>& /* groups */)> onChangeContactRequest;
	};
}
