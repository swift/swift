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
#include <Swiften/Elements/VCard.h>
#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/Roster/XMPPRosterItem.h>

namespace Swift {
	class UIEventStream;
	class ContactEditWindowFactory;
	class ContactEditWindow;
	class RosterController;
	class VCardManager;

	class ContactEditController {
		public:
			ContactEditController(RosterController* rosterController, VCardManager* vcardManager, ContactEditWindowFactory* contactEditWindowFactory, UIEventStream* uiEventStream);
			~ContactEditController();

			void setAvailable(bool b);

		public:
			static std::vector<std::string> nameSuggestionsFromVCard(VCard::ref vcard);

		private:
			void handleRemoveContactRequest();
			void handleChangeContactRequest(const std::string& name, const std::set<std::string>& groups);

		private:
			void handleUIEvent(UIEvent::ref event);
			void handleVCardChanged(const JID& jid, VCard::ref vcard);

		private:
			boost::optional<XMPPRosterItem> currentContact;
			RosterController* rosterController;
			VCardManager* vcardManager;
			ContactEditWindowFactory* contactEditWindowFactory;
			UIEventStream* uiEventStream;
			JID jid;
			ContactEditWindow* contactEditWindow;
	};
}

