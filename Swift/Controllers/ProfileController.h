/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIEvents/UIEvent.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/VCards/SetVCardRequest.h>

namespace Swift {
	class UIEventStream;
	class ProfileWindowFactory;
	class ProfileWindow;
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
			void updateDialogStatus();

		private:
			VCardManager* vcardManager;
			ProfileWindowFactory* profileWindowFactory;
			UIEventStream* uiEventStream;
			bool available;
			SetVCardRequest::ref pendingSetVCardRequest;
			ProfileWindow* profileWindow;
			bool gettingVCard;
	};
}

