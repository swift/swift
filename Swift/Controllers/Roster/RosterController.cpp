/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Roster/RosterController.h"

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/UIInterfaces/MainWindow.h"
#include "Swift/Controllers/UIInterfaces/MainWindowFactory.h"
#include "Swiften/Client/NickResolver.h"
#include "Swiften/Roster/GetRosterRequest.h"
#include "Swiften/Roster/SetRosterRequest.h"
#include "Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h"
#include "Swift/Controllers/XMPPEvents/ErrorEvent.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Presence/SubscriptionManager.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/SetPresence.h"
#include "Swift/Controllers/Roster/AppearOffline.h"
#include "Swift/Controllers/Roster/SetAvatar.h"
#include "Swift/Controllers/Roster/SetName.h"
#include "Swift/Controllers/Roster/OfflineRosterFilter.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/Roster/XMPPRosterItem.h"
#include "Swift/Controllers/UIEvents/AddContactUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameRosterItemUIEvent.h"
#include "Swift/Controllers/UIEvents/RenameGroupUIEvent.h"
#include "Swift/Controllers/UIEvents/SendFileUIEvent.h"
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/Client/NickManager.h>
#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/format.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swift/Controllers/SettingConstants.h>

namespace Swift {

/**
 * The controller does not gain ownership of these parameters.
 */
RosterController::RosterController(const JID& jid, XMPPRoster* xmppRoster, AvatarManager* avatarManager, MainWindowFactory* mainWindowFactory, NickManager* nickManager, NickResolver* nickResolver, PresenceOracle* presenceOracle, SubscriptionManager* subscriptionManager, EventController* eventController, UIEventStream* uiEventStream, IQRouter* iqRouter, SettingsProvider* settings, EntityCapsProvider* entityCapsManager, FileTransferOverview* fileTransferOverview)
 : myJID_(jid), xmppRoster_(xmppRoster), mainWindowFactory_(mainWindowFactory), mainWindow_(mainWindowFactory_->createMainWindow(uiEventStream)), roster_(new Roster()), offlineFilter_(new OfflineRosterFilter()), nickManager_(nickManager), nickResolver_(nickResolver), uiEventStream_(uiEventStream), entityCapsManager_(entityCapsManager), ftOverview_(fileTransferOverview) {
	assert(fileTransferOverview);
	iqRouter_ = iqRouter;
	presenceOracle_ = presenceOracle;
	subscriptionManager_ = subscriptionManager;
	eventController_ = eventController;
	settings_ = settings;
	expandiness_ = new RosterGroupExpandinessPersister(roster_, settings);
	mainWindow_->setRosterModel(roster_);
	
	changeStatusConnection_ = mainWindow_->onChangeStatusRequest.connect(boost::bind(&RosterController::handleChangeStatusRequest, this, _1, _2));
	signOutConnection_ = mainWindow_->onSignOutRequest.connect(boost::bind(boost::ref(onSignOutRequest)));
	xmppRoster_->onJIDAdded.connect(boost::bind(&RosterController::handleOnJIDAdded, this, _1));
	xmppRoster_->onJIDUpdated.connect(boost::bind(&RosterController::handleOnJIDUpdated, this, _1, _2, _3));
	xmppRoster_->onJIDRemoved.connect(boost::bind(&RosterController::handleOnJIDRemoved, this, _1));
	xmppRoster_->onRosterCleared.connect(boost::bind(&RosterController::handleRosterCleared, this));
	subscriptionManager_->onPresenceSubscriptionRequest.connect(boost::bind(&RosterController::handleSubscriptionRequest, this, _1, _2));
	presenceOracle_->onPresenceChange.connect(boost::bind(&RosterController::handleIncomingPresence, this, _1));
	uiEventConnection_ = uiEventStream->onUIEvent.connect(boost::bind(&RosterController::handleUIEvent, this, _1));
	avatarManager_ = avatarManager;
	avatarManager_->onAvatarChanged.connect(boost::bind(&RosterController::handleAvatarChanged, this, _1));
	mainWindow_->setMyAvatarPath(avatarManager_->getAvatarPath(myJID_).string());

	nickManager_->onOwnNickChanged.connect(boost::bind(&MainWindow::setMyNick, mainWindow_, _1));
	mainWindow_->setMyJID(jid);
	mainWindow_->setMyNick(nickManager_->getOwnNick());
	
	entityCapsManager_->onCapsChanged.connect(boost::bind(&RosterController::handleOnCapsChanged, this, _1));

	settings_->onSettingChanged.connect(boost::bind(&RosterController::handleSettingChanged, this, _1));

	handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
}


RosterController::~RosterController() {	
	settings_->onSettingChanged.disconnect(boost::bind(&RosterController::handleSettingChanged, this, _1));
	nickManager_->onOwnNickChanged.disconnect(boost::bind(&MainWindow::setMyNick, mainWindow_, _1));
	
	delete offlineFilter_;
	delete expandiness_;

	mainWindow_->setRosterModel(NULL);
	if (mainWindow_->canDelete()) {
		delete mainWindow_;
	}
	delete roster_;
	
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

void RosterController::handleChangeStatusRequest(StatusShow::Type show, const std::string &statusText) {
	onChangeStatusRequest(show, statusText);
}

void RosterController::handleOnJIDAdded(const JID& jid) {
	std::vector<std::string> groups = xmppRoster_->getGroupsForJID(jid);
	std::string name = nickResolver_->jidToNick(jid);
	if (!groups.empty()) {
		foreach(const std::string& group, groups) {
			roster_->addContact(jid, jid, name, group, avatarManager_->getAvatarPath(jid).string());
		}
	} 
	else {
		roster_->addContact(jid, jid, name, QT_TRANSLATE_NOOP("", "Contacts"), avatarManager_->getAvatarPath(jid).string());
	}
	applyAllPresenceTo(jid);
}

void RosterController::applyAllPresenceTo(const JID& jid) {
	foreach (Presence::ref presence, presenceOracle_->getAllPresence(jid)) {
		roster_->applyOnItems(SetPresence(presence));
	}
}

void RosterController::handleRosterCleared() {
	roster_->removeAll();
}

void RosterController::handleOnJIDRemoved(const JID& jid) {
	roster_->removeContact(jid);
}

void RosterController::handleOnJIDUpdated(const JID& jid, const std::string& oldName, const std::vector<std::string>& passedOldGroups) {
	if (oldName != xmppRoster_->getNameForJID(jid)) {
		roster_->applyOnItems(SetName(nickResolver_->jidToNick(jid), jid));
	}
	std::vector<std::string> groups = xmppRoster_->getGroupsForJID(jid);
	std::vector<std::string> oldGroups = passedOldGroups;
	std::string name = nickResolver_->jidToNick(jid);
	std::string contactsGroup = QT_TRANSLATE_NOOP("", "Contacts");
	if (oldGroups.empty()) {
		oldGroups.push_back(contactsGroup);
	}
	if (groups.empty()) {
		groups.push_back(contactsGroup);
	}
	foreach(const std::string& group, groups) {
		if (std::find(oldGroups.begin(), oldGroups.end(), group) == oldGroups.end()) {
			roster_->addContact(jid, jid, name, group, avatarManager_->getAvatarPath(jid).string());
		}
	} 
	foreach(const std::string& group, oldGroups) {
		if (std::find(groups.begin(), groups.end(), group) == groups.end()) {
			roster_->removeContactFromGroup(jid, group);
			if (roster_->getGroup(group)->getChildren().size() == 0) {
				roster_->removeGroup(group);
			}
		}
	}
	applyAllPresenceTo(jid);
}

void RosterController::handleSettingChanged(const std::string& settingPath) {
	if (settingPath == SettingConstants::SHOW_OFFLINE.getKey()) {
		handleShowOfflineToggled(settings_->getSetting(SettingConstants::SHOW_OFFLINE));
	}
}

void RosterController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	if (boost::shared_ptr<AddContactUIEvent> addContactEvent = boost::dynamic_pointer_cast<AddContactUIEvent>(event)) {
		RosterItemPayload item;
		item.setName(addContactEvent->getName());
		item.setJID(addContactEvent->getJID());
		item.setGroups(std::vector<std::string>(addContactEvent->getGroups().begin(), addContactEvent->getGroups().end()));
		boost::shared_ptr<RosterPayload> roster(new RosterPayload());
		roster->addItem(item);
		SetRosterRequest::ref request = SetRosterRequest::create(roster, iqRouter_);
		request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
		request->send();
		subscriptionManager_->requestSubscription(addContactEvent->getJID());
	}
	else if (boost::shared_ptr<RemoveRosterItemUIEvent> removeEvent = boost::dynamic_pointer_cast<RemoveRosterItemUIEvent>(event)) {
		RosterItemPayload item(removeEvent->getJID(), "", RosterItemPayload::Remove);
		boost::shared_ptr<RosterPayload> roster(new RosterPayload());
		roster->addItem(item);
		SetRosterRequest::ref request = SetRosterRequest::create(roster, iqRouter_);
		request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
		request->send();

	}
	else if (boost::shared_ptr<RenameRosterItemUIEvent> renameEvent = boost::dynamic_pointer_cast<RenameRosterItemUIEvent>(event)) {
		JID contact(renameEvent->getJID());
		RosterItemPayload item(contact, renameEvent->getNewName(), xmppRoster_->getSubscriptionStateForJID(contact));
		item.setGroups(xmppRoster_->getGroupsForJID(contact));
		boost::shared_ptr<RosterPayload> roster(new RosterPayload());
		roster->addItem(item);
		SetRosterRequest::ref request = SetRosterRequest::create(roster, iqRouter_);
		request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
		request->send();
	}
	else if (boost::shared_ptr<RenameGroupUIEvent> renameGroupEvent = boost::dynamic_pointer_cast<RenameGroupUIEvent>(event)) {
		std::vector<XMPPRosterItem> items = xmppRoster_->getItems();
		std::string group = renameGroupEvent->getGroup();
		// FIXME: We should handle contacts groups specially to avoid clashes
		if (group == QT_TRANSLATE_NOOP("", "Contacts")) {
			group = "";
		}
		foreach(XMPPRosterItem& item, items) {
			std::vector<std::string> groups = item.getGroups();
			if ( (group.empty() && groups.empty()) || std::find(groups.begin(), groups.end(), group) != groups.end()) {
				groups.erase(std::remove(groups.begin(), groups.end(), group), groups.end());
				if (std::find(groups.begin(), groups.end(), renameGroupEvent->getNewName()) == groups.end()) {
					groups.push_back(renameGroupEvent->getNewName());
				}
				item.setGroups(groups);
				updateItem(item);
			}
		}
	}
	else if (boost::shared_ptr<SendFileUIEvent> sendFileEvent = boost::dynamic_pointer_cast<SendFileUIEvent>(event)) {
		//TODO add send file dialog to ChatView of receipient jid
		ftOverview_->sendFile(sendFileEvent->getJID(), sendFileEvent->getFilename());
	}
}

void RosterController::setContactGroups(const JID& jid, const std::vector<std::string>& groups) {
	updateItem(XMPPRosterItem(jid, xmppRoster_->getNameForJID(jid), groups, xmppRoster_->getSubscriptionStateForJID(jid)));
}

void RosterController::updateItem(const XMPPRosterItem& item) {
	RosterItemPayload itemPayload(item.getJID(), item.getName(), item.getSubscription());
	itemPayload.setGroups(item.getGroups());

	RosterPayload::ref roster = boost::make_shared<RosterPayload>();
	roster->addItem(itemPayload);

	SetRosterRequest::ref request = SetRosterRequest::create(roster, iqRouter_);
	request->onResponse.connect(boost::bind(&RosterController::handleRosterSetError, this, _1, roster));
	request->send();
}

void RosterController::handleRosterSetError(ErrorPayload::ref error, boost::shared_ptr<RosterPayload> rosterPayload) {
	if (!error) {
		return;
	}
	std::string text = str(format(QT_TRANSLATE_NOOP("", "Server %1% rejected contact list change to item '%2%'")) % myJID_.getDomain() % rosterPayload->getItems()[0].getJID().toString());
	if (!error->getText().empty()) {
		text += ": " + error->getText();
	}
	boost::shared_ptr<ErrorEvent> errorEvent(new ErrorEvent(JID(myJID_.getDomain()), text));
	eventController_->handleIncomingEvent(errorEvent);
}

void RosterController::handleIncomingPresence(Presence::ref newPresence) {
	if (newPresence->getType() == Presence::Error) {
		return;
	}
	roster_->applyOnItems(SetPresence(newPresence));
}

void RosterController::handleSubscriptionRequest(const JID& jid, const std::string& message) {
	if (xmppRoster_->containsJID(jid) && (xmppRoster_->getSubscriptionStateForJID(jid) == RosterItemPayload::To || xmppRoster_->getSubscriptionStateForJID(jid) == RosterItemPayload::Both)) {
		subscriptionManager_->confirmSubscription(jid);
		return;
	}
	SubscriptionRequestEvent* eventPointer = new SubscriptionRequestEvent(jid, message);
	eventPointer->onAccept.connect(boost::bind(&RosterController::handleSubscriptionRequestAccepted, this, eventPointer));
	eventPointer->onDecline.connect(boost::bind(&RosterController::handleSubscriptionRequestDeclined, this, eventPointer));
	boost::shared_ptr<StanzaEvent> event(eventPointer);
	eventController_->handleIncomingEvent(event);
}

void RosterController::handleSubscriptionRequestAccepted(SubscriptionRequestEvent* event) {
	subscriptionManager_->confirmSubscription(event->getJID());
	if (!xmppRoster_->containsJID(event->getJID()) || xmppRoster_->getSubscriptionStateForJID(event->getJID()) == RosterItemPayload::None || xmppRoster_->getSubscriptionStateForJID(event->getJID()) == RosterItemPayload::From) {
		subscriptionManager_->requestSubscription(event->getJID());
	}
}

void RosterController::handleSubscriptionRequestDeclined(SubscriptionRequestEvent* event) {
	subscriptionManager_->cancelSubscription(event->getJID());
}

void RosterController::handleAvatarChanged(const JID& jid) {
	std::string path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path));
	if (jid.equals(myJID_, JID::WithoutResource)) {
		mainWindow_->setMyAvatarPath(path);
	}
}

boost::optional<XMPPRosterItem> RosterController::getItem(const JID& jid) const {
	return xmppRoster_->getItem(jid);
}

std::set<std::string> RosterController::getGroups() const {
	return xmppRoster_->getGroups();
}

void RosterController::handleOnCapsChanged(const JID& jid) {
	DiscoInfo::ref info = entityCapsManager_->getCaps(jid);
	if (info) {
		std::set<ContactRosterItem::Feature> features;
		if (info->hasFeature(DiscoInfo::JingleFeature) && info->hasFeature(DiscoInfo::JingleFTFeature) && info->hasFeature(DiscoInfo::JingleTransportsIBBFeature)) {
			features.insert(ContactRosterItem::FileTransferFeature);
		}
		roster_->setAvailableFeatures(jid, features);
	}
}

}
