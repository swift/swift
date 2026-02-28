/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Roster/XMPPRosterItem.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

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

