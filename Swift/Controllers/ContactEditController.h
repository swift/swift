/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <set>
#include <boost/optional.hpp>

#include <Swiften/JID/JID.h>
#include <string>
#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/Roster/XMPPRosterItem.h>

namespace Swift {
	class UIEventStream;
	class ContactEditWindowFactory;
	class ContactEditWindow;
	class RosterController;

	class ContactEditController {
		public:
			ContactEditController(RosterController* rosterController, ContactEditWindowFactory* contactEditWindowFactory, UIEventStream* uiEventStream);
			~ContactEditController();

			void setAvailable(bool b);

		private:
			void handleRemoveContactRequest();
			void handleChangeContactRequest(const std::string& name, const std::set<std::string>& groups);

		private:
			void handleUIEvent(UIEvent::ref event);

		private:
			boost::optional<XMPPRosterItem> currentContact;
			RosterController* rosterController;
			ContactEditWindowFactory* contactEditWindowFactory;
			UIEventStream* uiEventStream;
			ContactEditWindow* contactEditWindow;
	};
}

