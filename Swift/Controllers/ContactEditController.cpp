/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/ContactEditController.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swift/Controllers/UIEvents/RequestContactEditorUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h>
#include <Swift/Controllers/UIInterfaces/ContactEditWindowFactory.h>
#include <Swift/Controllers/RosterController.h>


namespace Swift {

ContactEditController::ContactEditController(RosterController* rosterController, ContactEditWindowFactory* contactEditWindowFactory, UIEventStream* uiEventStream) : rosterController(rosterController), contactEditWindowFactory(contactEditWindowFactory), uiEventStream(uiEventStream), contactEditWindow(NULL) {
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
	contactEditWindow->setContact(currentContact->getJID(), currentContact->getName(), currentContact->getGroups(), rosterController->getGroups());
	contactEditWindow->show();
}

void ContactEditController::setAvailable(bool b) {
	if (contactEditWindow) {
		contactEditWindow->setEnabled(b);
	}
}

void ContactEditController::handleRemoveContactRequest() {
	assert(currentContact);
	uiEventStream->send(boost::make_shared<RemoveRosterItemUIEvent>(currentContact->getJID()));
	contactEditWindow->hide();
}

void ContactEditController::handleChangeContactRequest(const String& name, const std::vector<String>& groups) {
	std::vector<String> oldGroupsVector = currentContact->getGroups();
	std::set<String> oldGroups(oldGroupsVector.begin(), oldGroupsVector.end());
	std::set<String> newGroups(groups.begin(), groups.end());
	if (oldGroups != newGroups || currentContact->getName() != name) {
		XMPPRosterItem newContact(*currentContact);
		newContact.setName(name);
		newContact.setGroups(groups);
		rosterController->updateItem(newContact);
	}
	contactEditWindow->hide();
}

}
