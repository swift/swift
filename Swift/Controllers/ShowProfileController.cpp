/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "ShowProfileController.h"

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/VCards/VCardManager.h>

#include <Swift/Controllers/UIEvents/ShowProfileForRosterItemUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ProfileWindowFactory.h>

namespace Swift {

ShowProfileController::ShowProfileController(VCardManager* vcardManager, ProfileWindowFactory* profileWindowFactory, UIEventStream* uiEventStream) : vcardManager(vcardManager), profileWindowFactory(profileWindowFactory), uiEventStream(uiEventStream) {
	uiEventStream->onUIEvent.connect(boost::bind(&ShowProfileController::handleUIEvent, this, _1));
	vcardManager->onVCardChanged.connect(boost::bind(&ShowProfileController::handleVCardChanged, this, _1, _2));
}

ShowProfileController::~ShowProfileController() {
	typedef std::pair<JID, ProfileWindow*> JIDProfileWindowPair;
	foreach(const JIDProfileWindowPair& jidWndPair, openedProfileWindows) {
		jidWndPair.second->onWindowClosed.disconnect(boost::bind(&ShowProfileController::handleProfileWindowClosed, this, _1));
		delete jidWndPair.second;
	}

	vcardManager->onVCardChanged.disconnect(boost::bind(&ShowProfileController::handleVCardChanged, this, _1, _2));
	uiEventStream->onUIEvent.disconnect(boost::bind(&ShowProfileController::handleUIEvent, this, _1));
}

void ShowProfileController::handleUIEvent(UIEvent::ref event) {
	ShowProfileForRosterItemUIEvent::ref showProfileEvent = boost::dynamic_pointer_cast<ShowProfileForRosterItemUIEvent>(event);
	if (!showProfileEvent) {
		return;
	}

	if (openedProfileWindows.find(showProfileEvent->getJID()) == openedProfileWindows.end()) {
		ProfileWindow* newProfileWindow = profileWindowFactory->createProfileWindow();
		newProfileWindow->setJID(showProfileEvent->getJID());
		newProfileWindow->onWindowClosed.connect(boost::bind(&ShowProfileController::handleProfileWindowClosed, this, _1));
		openedProfileWindows[showProfileEvent->getJID()] = newProfileWindow;
		VCard::ref vcard = vcardManager->getVCardAndRequestWhenNeeded(showProfileEvent->getJID());
		if (vcard) {
			newProfileWindow->setVCard(vcard);
		} else {
			newProfileWindow->setProcessing(true);
		}
		newProfileWindow->show();
	} else {
		openedProfileWindows[showProfileEvent->getJID()]->show();
	}
}

void ShowProfileController::handleVCardChanged(const JID& jid, VCard::ref vcard) {
	if (openedProfileWindows.find(jid) == openedProfileWindows.end()) {
		return;
	}

	ProfileWindow* profileWindow = openedProfileWindows[jid];
	profileWindow->setVCard(vcard);
	profileWindow->setProcessing(false);
	profileWindow->show();
}

void ShowProfileController::handleProfileWindowClosed(const JID& profileJid) {
	ProfileWindow* profileWindow = openedProfileWindows[profileJid];
	openedProfileWindows.erase(profileJid);
	delete profileWindow;
}

}
