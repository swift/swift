/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class VCardManager;
    class ProfileWindow;
    class ProfileWindowFactory;
    class UIEventStream;

    class ShowProfileController {
        public:
            ShowProfileController(VCardManager* vcardManager, ProfileWindowFactory* profileWindowFactory, UIEventStream* uiEventStream);
            ~ShowProfileController();

        private:
            void handleUIEvent(UIEvent::ref event);
            void handleVCardChanged(const JID&, VCard::ref);
            void handleProfileWindowAboutToBeClosed(const JID& profileJid);
            void handleVCardRetrievalError(const JID& jid, ErrorPayload::ref error);

        private:
            VCardManager* vcardManager;
            ProfileWindowFactory* profileWindowFactory;
            UIEventStream* uiEventStream;
            std::map<JID, ProfileWindow*> openedProfileWindows;
    };
}
