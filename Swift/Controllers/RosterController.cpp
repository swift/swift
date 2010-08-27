/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/RosterController.h"

#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/UIInterfaces/MainWindow.h"
#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"
#include "Swiften/Queries/Requests/SetRosterRequest.h"
#include "Swiften/Events/SubscriptionRequestEvent.h"
#include "Swiften/Events/ErrorEvent.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swift/Controllers/EventController.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/SetPresence.h"
#include "Swiften/Roster/AppearOffline.h"
#include "Swiften/Roster/SetAvatar.h"
#include "Swiften/Roster/SetName.h"
#include "Swiften/Roster/OfflineRosterFilter.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swift/Controllers/UIEvents/AddContactUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"


namespace Swift {
	
/**
 * The controller does not gain ownership of these parameters.
 */
RosterController::RosterController(const JID& jid, boost::shared_ptr<XMPPRoster> xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, EventController* eventController, UIEventStream* uiEventStream, IQRouter* iqRouter)
 : myJID_(jid), xmppRoster_(xmppRoster), mainWindowFactory_(mainWindowFactory), mainWindow_(mainWindowFactory_->createMainWindow(uiEventStream)), roster_(new Roster()), offlineFilter_(new OfflineRosterFilter()) {
	iqRouter_ = iqRouter;
	presenceOracle_ = presenceOracle;
	eventController_ = eventController;
	roster_->addFilter(offlineFilter_);
	mainWindow_->setRosterModel(roster_);
	
	changeStatusConnection_ = mainWindow_->onChangeStatusRequest.connect(boost::bind(&RosterController::handleChangeStatusRequest, this, _1, _2));
	showOfflineConnection_ = mainWindow_->onShowOfflineToggled.connect(boost::bind(&RosterController::handleShowOfflineToggled, this, _1));
	signOutConnection_ = mainWindow_->onSignOutRequest.connect(boost::bind(boost::ref(onSignOutRequest)));
	xmppRoster_->onJIDAdded.connect(boost::bind(&RosterController::handleOnJIDAdded, this, _1));
	xmppRoster_->onJIDUpdated.connect(boost::bind(&RosterController::handleOnJIDUpdated, this, _1, _2, _3));
	xmppRoster_->onJIDRemoved.connect(boost::bind(&RosterController::handleOnJIDRemoved, this, _1));
	presenceOracle_->onPresenceSubscriptionRequest.connect(boost::bind(&RosterController::handleSubscriptionRequest, this, _1, _2));
	presenceOracle_->onPresenceChange.connect(boost::bind(&RosterController::handleIncomingPresence, this, _1, _2));
	uiEventConnection_ = uiEventStream->onUIEvent.connect(boost::bind(&RosterController::handleUIEvent, this, _1));
	avatarManager_ = NULL;
	setAvatarManager(avatarManager);
	setNickResolver(nickResolver);
	
}

RosterController::~RosterController() {
	delete offlineFilter_;

}

void RosterController::setNickResolver(NickResolver* nickResolver) {
	nickResolver_ = nickResolver;
	if (nickResolver_ != NULL) {
		handleOwnNickChanged(nickResolver_->jidToNick(myJID_));

		nickResolver_->onOwnNickChanged.connect(boost::bind(&RosterController::handleOwnNickChanged, this, _1));
	}
}

void RosterController::handleOwnNickChanged(const String& nick) {
	mainWindow_->setMyName(nick);
}

void RosterController::setAvatarManager(AvatarManager* avatarManager) {
	if (avatarManager_ != NULL) {
		//FIXME: disconnect old signal;
	}
	avatarManager_ = avatarManager;
	if (avatarManager != NULL) {
		avatarManager_->onAvatarChanged.connect(boost::bind(&RosterController::handleAvatarChanged, this, _1, _2));
		mainWindow_->setMyAvatarPath(avatarManager_->getAvatarPath(myJID_).string());
	}
}

void RosterController::setEnabled(bool enabled) {
	if (!enabled) {
		roster_->applyOnItems(AppearOffline());
	}
}

void RosterController::handleShowOfflineToggled(bool state) {
	if (state) {
		roster_->removeFilter(offlineFilter_);
	} else {
		roster_->addFilter(offlineFilter_);
	}
}

void RosterController::handleChangeStatusRequest(StatusShow::Type show, const String &statusText) {
	onChangeStatusRequest(show, statusText);
}

void RosterController::handleOnJIDAdded(const JID& jid) {
	std::vector<String> groups = xmppRoster_->getGroupsForJID(jid);
	String name = nickResolver_->jidToNick(jid);
	if (!groups.empty()) {
		foreach(const String& group, groups) {
			roster_->addContact(jid, jid, name, group);
		}
	} else {
		roster_->addContact(jid, jid, name, "Contacts");
	}
}

void RosterController::handleOnJIDRemoved(const JID& jid) {
	roster_->removeContact(jid);
}

void RosterController::handleOnJIDUpdated(const JID& jid, const String& oldName, const std::vector<String> passedOldGroups) {
	if (oldName != xmppRoster_->getNameForJID(jid)) {
		roster_->applyOnItems(SetName(nickResolver_->jidToNick(jid), jid));
		return;
	}
	std::vector<String> groups = xmppRoster_->getGroupsForJID(jid);
	std::vector<String> oldGroups = passedOldGroups;
	String name = nickResolver_->jidToNick(jid);
	String contactsGroup = "Contacts";
	if (oldGroups.empty()) {
		oldGroups.push_back(contactsGroup);
	}
	if (groups.empty()) {
		groups.push_back(contactsGroup);
	}
	foreach(const String& group, groups) {
		if (std::find(oldGroups.begin(), oldGroups.end(), group) == oldGroups.end()) {
			roster_->addContact(jid, jid, xmppRoster_->getNameForJID(jid), group);
		}
	} 
	foreach(const String& group, oldGroups) {
		if (std::find(groups.begin(), groups.end(), group) == groups.end()) {
			roster_->removeContactFromGroup(jid, group);
		}
	}
	
}

void RosterController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<AddContactUIEvent> addContactEvent = boost::dynamic_pointer_cast<AddContactUIEvent>(event);
	if (addContactEvent) {
		RosterItemPayload item;
		item.setName(addContactEvent->getName());
		item.setJID(addContactEvent->getJID());
		boost::shared_ptr<RosterPayload> roster(new RosterPayload());
		roster->addItem(item);
		boost::shared_ptr<SetRosterRequest> request(new SetRosterRequest(roster, iqRouter_));
		request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
		request->send();
		presenceOracle_->requestSubscription(addContactEvent->getJID());
		return;
	}
	boost::shared_ptr<RemoveRosterItemUIEvent> removeEvent = boost::dynamic_pointer_cast<RemoveRosterItemUIEvent>(event);
	if (removeEvent) {
		RosterItemPayload item(removeEvent->getJID(), "", RosterItemPayload::Remove);
		boost::shared_ptr<RosterPayload> roster(new RosterPayload());
		roster->addItem(item);
		boost::shared_ptr<SetRosterRequest> request(new SetRosterRequest(roster, iqRouter_));
		request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
		request->send();

		return;
	}

}

void RosterController::handleRosterSetError(boost::optional<ErrorPayload> error, boost::shared_ptr<RosterPayload> rosterPayload) {
	if (!error) {
		return;
	}
	String text = "Server " + myJID_.getDomain() + " rejected roster change to item '" + rosterPayload->getItems()[0].getJID() + "'";
	if (!error->getText().isEmpty()) {
		text += ": " + error->getText();
	}
	boost::shared_ptr<ErrorEvent> errorEvent(new ErrorEvent(JID(myJID_.getDomain()), text));
	eventController_->handleIncomingEvent(errorEvent);
}

void RosterController::handleIncomingPresence(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> /*oldPresence*/) {
	roster_->applyOnItems(SetPresence(newPresence));
}

void RosterController::handleSubscriptionRequest(const JID& jid, const String& message) {
	if (xmppRoster_->containsJID(jid) && (xmppRoster_->getSubscriptionStateForJID(jid) == RosterItemPayload::To || xmppRoster_->getSubscriptionStateForJID(jid) == RosterItemPayload::Both)) {
		presenceOracle_->confirmSubscription(jid);
		return;
	}
	SubscriptionRequestEvent* eventPointer = new SubscriptionRequestEvent(jid, message);
	eventPointer->onAccept.connect(boost::bind(&RosterController::handleSubscriptionRequestAccepted, this, eventPointer));
	eventPointer->onDecline.connect(boost::bind(&RosterController::handleSubscriptionRequestDeclined, this, eventPointer));
	boost::shared_ptr<StanzaEvent> event(eventPointer);
	eventController_->handleIncomingEvent(event);
}

void RosterController::handleSubscriptionRequestAccepted(SubscriptionRequestEvent* event) {
	presenceOracle_->confirmSubscription(event->getJID());
	if (!xmppRoster_->containsJID(event->getJID()) || xmppRoster_->getSubscriptionStateForJID(event->getJID()) == RosterItemPayload::None || xmppRoster_->getSubscriptionStateForJID(event->getJID()) == RosterItemPayload::From) {
		presenceOracle_->requestSubscription(event->getJID());
	}
}

void RosterController::handleSubscriptionRequestDeclined(SubscriptionRequestEvent* event) {
	presenceOracle_->cancelSubscription(event->getJID());
}

void RosterController::handleAvatarChanged(const JID& jid, const String&) {
	String path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path));
	if (jid.equals(myJID_, JID::WithoutResource)) {
		mainWindow_->setMyAvatarPath(path);
	}
}

}
