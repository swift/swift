/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/VCards/SetVCardRequest.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class ProfileWindow;
    class ProfileWindowFactory;
    class UIEventStream;
    class VCardManager;

    class ProfileController {
        public:
            ProfileController(VCardManager* vcardManager, ProfileWindowFactory* profileWindowFactory, UIEventStream* uiEventStream);
            ~ProfileController();

            void setAvailable(bool b);

        private:
            void handleUIEvent(UIEvent::ref event);
            void handleVCardChangeRequest(VCard::ref vcard);
            void handleSetVCardResponse(ErrorPayload::ref);
            void handleOwnVCardChanged(VCard::ref vcard);
            void handleVCardRetrievalError(const JID& jid, ErrorPayload::ref error);
            void handleProfileWindowAboutToBeClosed(const JID&);
            void updateDialogStatus();

        private:
            VCardManager* vcardManager;
            ProfileWindowFactory* profileWindowFactory;
            UIEventStream* uiEventStream;
            bool available;
            SetVCardRequest::ref pendingSetVCardRequest;
            ProfileWindow* profileWindow;
            bool gettingVCard;
            VCard::ref vcardBeforeEdit;
    };
}

