/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/VCard.h>

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

		private:
			VCardManager* vcardManager;
			ProfileWindowFactory* profileWindowFactory;
			UIEventStream* uiEventStream;
			std::map<JID, ProfileWindow*> openedProfileWindows;
	};
}
