/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/ContactEditController.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swift/Controllers/UIEvents/RequestContactEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h>
#include <Swift/Controllers/UIInterfaces/ContactEditWindowFactory.h>
#include <Swift/Controllers/Roster/RosterController.h>
#include <Swiften/VCards/VCardManager.h>


namespace Swift {

ContactEditController::ContactEditController(RosterController* rosterController, VCardManager* vcardManager, ContactEditWindowFactory* contactEditWindowFactory, UIEventStream* uiEventStream) : rosterController(rosterController), vcardManager(vcardManager), contactEditWindowFactory(contactEditWindowFactory), uiEventStream(uiEventStream), contactEditWindow(NULL) {
	uiEventStream->onUIEvent.connect(boost::bind(&ContactEditController::handleUIEvent, this, _1));
}

ContactEditController::~ContactEditController() {
	if (contactEditWindow) {
		contactEditWindow->onChangeContactRequest.disconnect(boost::bind(&ContactEditController::handleChangeContactRequest, this, _1, _2));
		contactEditWindow->onRemoveContactRequest.disconnect(boost::bind(&ContactEditController::handleRemoveContactRequest, this));
		delete contactEditWindow;
	}
	uiEventStream->onUIEvent.disconnect(boost::bind(&ContactEditController::handleUIEvent, this, _1));
}

void ContactEditController::handleUIEvent(UIEvent::ref event) {
	RequestContactEditorUIEvent::ref editEvent = boost::dynamic_pointer_cast<RequestContactEditorUIEvent>(event);
	if (!editEvent) {
		return;
	}

	if (!contactEditWindow) {
		contactEditWindow = contactEditWindowFactory->createContactEditWindow();
		contactEditWindow->onRemoveContactRequest.connect(boost::bind(&ContactEditController::handleRemoveContactRequest, this));
		contactEditWindow->onChangeContactRequest.connect(boost::bind(&ContactEditController::handleChangeContactRequest, this, _1, _2));
	}
	currentContact = rosterController->getItem(editEvent->getJID());
	assert(currentContact);
	jid = rosterController->getItem(editEvent->getJID())->getJID();
	contactEditWindow->setContact(jid, currentContact->getName(), currentContact->getGroups(), rosterController->getGroups());
	contactEditWindow->show();

	if (vcardManager) {
		VCard::ref vcard = vcardManager->getVCardAndRequestWhenNeeded(jid);
		if (vcard) {
			handleVCardChanged(jid, vcard);
		}
	}
}

void ContactEditController::handleVCardChanged(const JID &jid, VCard::ref vcard) {
	if (jid == this->jid) {
		contactEditWindow->setNameSuggestions(nameSuggestionsFromVCard(vcard));
	}
}

void ContactEditController::setAvailable(bool b) {
	if (contactEditWindow) {
		contactEditWindow->setEnabled(b);
	}
}

std::vector<std::string> ContactEditController::nameSuggestionsFromVCard(VCard::ref vcard) {
	std::vector<std::string> suggestions;
	if (!vcard->getNickname().empty()) {
		suggestions.push_back(vcard->getNickname());
	}
	if (!vcard->getFullName().empty()) {
		suggestions.push_back(vcard->getFullName());
	}
	if (!vcard->getGivenName().empty()) {
		std::string suggestedName;
		suggestedName = vcard->getGivenName();
		boost::algorithm::trim(suggestedName);
		suggestions.push_back(suggestedName);
	}
	return suggestions;
}

void ContactEditController::handleRemoveContactRequest() {
	assert(currentContact);
	uiEventStream->send(boost::make_shared<RemoveRosterItemUIEvent>(currentContact->getJID()));
	contactEditWindow->hide();
}

void ContactEditController::handleChangeContactRequest(const std::string& name, const std::set<std::string>& newGroups) {
	std::vector<std::string> oldGroupsVector = currentContact->getGroups();
	std::set<std::string> oldGroups(oldGroupsVector.begin(), oldGroupsVector.end());
	if (oldGroups != newGroups || currentContact->getName() != name) {
		XMPPRosterItem newContact(*currentContact);
		newContact.setName(name);
		newContact.setGroups(std::vector<std::string>(newGroups.begin(), newGroups.end()));
		rosterController->updateItem(newContact);
	}
	contactEditWindow->hide();
}

}
