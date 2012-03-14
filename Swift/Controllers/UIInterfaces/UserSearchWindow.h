/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"

#include <vector>
#include <string>

#include "Swiften/JID/JID.h"
#include "Swift/Controllers/Chat/UserSearchController.h"

namespace Swift {

	class UserSearchWindow {
		public:
			enum Type {AddContact, ChatToContact};
			virtual ~UserSearchWindow() {}

			virtual void clear() = 0;
			virtual void setResults(const std::vector<UserSearchResult>& results) = 0;
			virtual void setResultsForm(const Form::ref results) = 0;
			virtual void addSavedServices(const std::vector<JID>& services) = 0;
			virtual void setSelectedService(const JID& service) = 0;
			virtual void setServerSupportsSearch(bool support) = 0;
			virtual void setSearchError(bool support) = 0;
			virtual void setSearchFields(boost::shared_ptr<SearchPayload> fields) = 0;
			virtual void setNameSuggestions(const std::vector<std::string>& suggestions) = 0;
			virtual void show() = 0;

			boost::signal<void (const JID&)> onFormRequested;
			boost::signal<void (boost::shared_ptr<SearchPayload>, const JID&)> onSearchRequested;
			boost::signal<void (const JID&)> onNameSuggestionRequested;
	};
}
