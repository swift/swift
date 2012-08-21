/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/Controllers/HistoryViewController.h>

#include <Swift/Controllers/UIInterfaces/HistoryWindowFactory.h>
#include <Swift/Controllers/UIEvents/RequestHistoryUIEvent.h>
#include <Swift/Controllers/HistoryController.h>
#include <Swiften/History/HistoryMessage.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Avatars/AvatarManager.h>
#include <Swift/Controllers/Roster/SetPresence.h>
#include <Swift/Controllers/Roster/SetAvatar.h>

namespace Swift {
	static const std::string category[] = { "Contacts", "MUC", "Contacts" };

HistoryViewController::HistoryViewController(
		const JID& selfJID,
		UIEventStream* uiEventStream,
		HistoryController* historyController,
		NickResolver* nickResolver,
		AvatarManager* avatarManager,
		PresenceOracle* presenceOracle,
		HistoryWindowFactory* historyWindowFactory) :
			selfJID_(selfJID),
			uiEventStream_(uiEventStream),
			historyController_(historyController),
			nickResolver_(nickResolver),
			avatarManager_(avatarManager),
			presenceOracle_(presenceOracle),
			historyWindowFactory_(historyWindowFactory),
			historyWindow_(NULL),
			selectedItem_(NULL),
			currentResultDate_(boost::gregorian::not_a_date_time) {
	uiEventStream_->onUIEvent.connect(boost::bind(&HistoryViewController::handleUIEvent, this, _1));

	roster_ = new Roster(false, true);
}

HistoryViewController::~HistoryViewController() {
	uiEventStream_->onUIEvent.disconnect(boost::bind(&HistoryViewController::handleUIEvent, this, _1));
	if (historyWindow_) {
		historyWindow_->onSelectedContactChanged.disconnect(boost::bind(&HistoryViewController::handleSelectedContactChanged, this, _1));
		historyWindow_->onReturnPressed.disconnect(boost::bind(&HistoryViewController::handleReturnPressed, this, _1));
		historyWindow_->onScrollReachedTop.disconnect(boost::bind(&HistoryViewController::handleScrollReachedTop, this, _1));
		historyWindow_->onScrollReachedBottom.disconnect(boost::bind(&HistoryViewController::handleScrollReachedBottom, this, _1));
		historyWindow_->onPreviousButtonClicked.disconnect(boost::bind(&HistoryViewController::handlePreviousButtonClicked, this));
		historyWindow_->onNextButtonClicked.disconnect(boost::bind(&HistoryViewController::handleNextButtonClicked, this));
		historyWindow_->onCalendarClicked.disconnect(boost::bind(&HistoryViewController::handleCalendarClicked, this, _1));
		historyController_->onNewMessage.disconnect(boost::bind(&HistoryViewController::handleNewMessage, this, _1));

		presenceOracle_->onPresenceChange.disconnect(boost::bind(&HistoryViewController::handlePresenceChanged, this, _1));
		avatarManager_->onAvatarChanged.disconnect(boost::bind(&HistoryViewController::handleAvatarChanged, this, _1));

		delete historyWindow_;
	}
	delete roster_;
}

void HistoryViewController::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
	boost::shared_ptr<RequestHistoryUIEvent> event = boost::dynamic_pointer_cast<RequestHistoryUIEvent>(rawEvent);
	if (event != NULL) {
		if (historyWindow_ == NULL) {
			historyWindow_ = historyWindowFactory_->createHistoryWindow(uiEventStream_);
			historyWindow_->onSelectedContactChanged.connect(boost::bind(&HistoryViewController::handleSelectedContactChanged, this, _1));
			historyWindow_->onReturnPressed.connect(boost::bind(&HistoryViewController::handleReturnPressed, this, _1));
			historyWindow_->onScrollReachedTop.connect(boost::bind(&HistoryViewController::handleScrollReachedTop, this, _1));
			historyWindow_->onScrollReachedBottom.connect(boost::bind(&HistoryViewController::handleScrollReachedBottom, this, _1));
			historyWindow_->onPreviousButtonClicked.connect(boost::bind(&HistoryViewController::handlePreviousButtonClicked, this));
			historyWindow_->onNextButtonClicked.connect(boost::bind(&HistoryViewController::handleNextButtonClicked, this));
			historyWindow_->onCalendarClicked.connect(boost::bind(&HistoryViewController::handleCalendarClicked, this, _1));
			historyController_->onNewMessage.connect(boost::bind(&HistoryViewController::handleNewMessage, this, _1));

			presenceOracle_->onPresenceChange.connect(boost::bind(&HistoryViewController::handlePresenceChanged, this, _1));
			avatarManager_->onAvatarChanged.connect(boost::bind(&HistoryViewController::handleAvatarChanged, this, _1));

			historyWindow_->setRosterModel(roster_);
		}

		// populate roster by doing an empty search
		handleReturnPressed(std::string());

		historyWindow_->activate();
	}
}

void HistoryViewController::handleSelectedContactChanged(RosterItem* newContact) {
	// FIXME: signal is triggerd twice.
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(newContact);

	if (contact && selectedItem_ != contact) {
		selectedItem_ = contact;
		historyWindow_->resetConversationView();
	}
	else {
		return;
	}

	JID contactJID = contact->getJID();

	std::vector<HistoryMessage> messages;
	for (int it = HistoryMessage::Chat; it <= HistoryMessage::PrivateMessage; it++) {
		HistoryMessage::Type type = static_cast<HistoryMessage::Type>(it);

		if (contacts_[type].count(contactJID)) {
			currentResultDate_ = *contacts_[type][contactJID].rbegin();
			selectedItemType_ = type;
			messages = historyController_->getMessagesFromDate(selfJID_, contactJID, type, currentResultDate_);
		}
	}

	historyWindow_->setDate(currentResultDate_);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, false);
	}
}

void HistoryViewController::handleNewMessage(const HistoryMessage& message) {
	JID contactJID = message.getFromJID().toBare() == selfJID_ ? message.getToJID() : message.getFromJID();

	JID displayJID;
	if (message.getType() == HistoryMessage::PrivateMessage) {
		displayJID = contactJID;
	}
	else {
		displayJID = contactJID.toBare();
	}

	// check current conversation
	if (selectedItem_ && selectedItem_->getJID() == displayJID) {
		if (historyWindow_->getLastVisibleDate() == message.getTime().date()) {
			addNewMessage(message, false);
		}
	}

	// check if the new message matches the query
	if (message.getMessage().find(historyWindow_->getSearchBoxText()) == std::string::npos) {
		return;
	}

	// update contacts
	if (!contacts_[message.getType()].count(displayJID)) {
		roster_->addContact(displayJID, displayJID, nickResolver_->jidToNick(displayJID), category[message.getType()], avatarManager_->getAvatarPath(displayJID).string());
	}

	contacts_[message.getType()][displayJID].insert(message.getTime().date());
}

void HistoryViewController::addNewMessage(const HistoryMessage& message, bool addAtTheTop) {
	bool senderIsSelf = message.getFromJID().toBare() == selfJID_;
	std::string avatarPath = avatarManager_->getAvatarPath(message.getFromJID()).string();

	std::string nick = message.getType() != HistoryMessage::Groupchat ? nickResolver_->jidToNick(message.getFromJID()) : message.getFromJID().getResource();
	historyWindow_->addMessage(message.getMessage(), nick, senderIsSelf, avatarPath, message.getTime(), addAtTheTop);
}

void HistoryViewController::handleReturnPressed(const std::string& keyword) {
	reset();

	for (int it = HistoryMessage::Chat; it <= HistoryMessage::PrivateMessage; it++) {
		HistoryMessage::Type type = static_cast<HistoryMessage::Type>(it);

		contacts_[type] = historyController_->getContacts(selfJID_, type, keyword);

		for (ContactsMap::const_iterator contact = contacts_[type].begin(); contact != contacts_[type].end(); contact++) {
			const JID& jid = contact->first;
			std::string nick;
			if (type == HistoryMessage::PrivateMessage) {
				nick = jid.toString();
			}
			else {
				nick = nickResolver_->jidToNick(jid);
			}
			roster_->addContact(jid, jid, nick, category[type], avatarManager_->getAvatarPath(jid).string());

			Presence::ref presence = getPresence(jid, type == HistoryMessage::Groupchat);

			if (presence.get()) {
				roster_->applyOnItem(SetPresence(presence, JID::WithoutResource), jid);
			}
		}
	}
}

void HistoryViewController::handleScrollReachedTop(const boost::gregorian::date& date) {
	if (!selectedItem_) {
		return;
	}

	std::vector<HistoryMessage> messages = historyController_->getMessagesFromPreviousDate(selfJID_, selectedItem_->getJID(), selectedItemType_, date);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, true);
	}
	historyWindow_->resetConversationViewTopInsertPoint();
}

void HistoryViewController::handleScrollReachedBottom(const boost::gregorian::date& date) {
	if (!selectedItem_) {
		return;
	}

	std::vector<HistoryMessage> messages = historyController_->getMessagesFromNextDate(selfJID_, selectedItem_->getJID(), selectedItemType_, date);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, false);
	}
}

void HistoryViewController::handleNextButtonClicked() {
	if (!selectedItem_) {
		return;
	}

	std::set<boost::gregorian::date>::iterator date = contacts_[selectedItemType_][selectedItem_->getJID()].find(currentResultDate_);

	if (*date == *contacts_[selectedItemType_][selectedItem_->getJID()].rbegin()) {
		return;
	}

	historyWindow_->resetConversationView();
	currentResultDate_ = *(++date);
	std::vector<HistoryMessage> messages = historyController_->getMessagesFromDate(selfJID_, selectedItem_->getJID(), selectedItemType_, currentResultDate_);
	historyWindow_->setDate(currentResultDate_);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, false);
	}
}

void HistoryViewController::handlePreviousButtonClicked() {
	if (!selectedItem_) {
		return;
	}

	std::set<boost::gregorian::date>::iterator date = contacts_[selectedItemType_][selectedItem_->getJID()].find(currentResultDate_);

	if (date == contacts_[selectedItemType_][selectedItem_->getJID()].begin()) {
		return;
	}

	historyWindow_->resetConversationView();
	currentResultDate_ = *(--date);
	std::vector<HistoryMessage> messages = historyController_->getMessagesFromDate(selfJID_, selectedItem_->getJID(), selectedItemType_, currentResultDate_);
	historyWindow_->setDate(currentResultDate_);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, false);
	}
}

void HistoryViewController::reset() {
	roster_->removeAll();
	contacts_.clear();
	selectedItem_ = NULL;
	historyWindow_->resetConversationView();
}

void HistoryViewController::handleCalendarClicked(const boost::gregorian::date& date) {
	if (!selectedItem_) {
		return;
	}

	boost::gregorian::date newDate;
	if (contacts_[selectedItemType_][selectedItem_->getJID()].count(date)) {
		newDate = date;
	}
	else if (date < currentResultDate_) {
		foreach(const boost::gregorian::date& current, contacts_[selectedItemType_][selectedItem_->getJID()]) {
			if (current > date) {
				newDate = current;
				break;
			}
		}
	}
	else {
		reverse_foreach(const boost::gregorian::date& current, contacts_[selectedItemType_][selectedItem_->getJID()]) {
			if (current < date) {
				newDate = current;
				break;
			}
		}
	}

	historyWindow_->setDate(newDate);
	if (newDate == currentResultDate_) {
		return;
	}
	currentResultDate_ = newDate;
	historyWindow_->resetConversationView();

	std::vector<HistoryMessage> messages = historyController_->getMessagesFromDate(selfJID_, selectedItem_->getJID(), selectedItemType_, currentResultDate_);
	historyWindow_->setDate(currentResultDate_);

	foreach (const HistoryMessage& message, messages) {
		addNewMessage(message, false);
	}
}

void HistoryViewController::handlePresenceChanged(Presence::ref presence) {
	JID jid = presence->getFrom();

	if (contacts_[HistoryMessage::Chat].count(jid.toBare())) {
		roster_->applyOnItems(SetPresence(presence, JID::WithoutResource));
		return;
	}

	if (contacts_[HistoryMessage::Groupchat].count(jid.toBare())) {
		Presence::ref availablePresence = boost::make_shared<Presence>(Presence());
		availablePresence->setFrom(jid.toBare());
		roster_->applyOnItems(SetPresence(availablePresence, JID::WithResource));
	}

	if (contacts_[HistoryMessage::PrivateMessage].count(jid)) {
		roster_->applyOnItems(SetPresence(presence, JID::WithResource));
	}
}

void HistoryViewController::handleAvatarChanged(const JID& jid) {
	std::string path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path));
}

Presence::ref HistoryViewController::getPresence(const JID& jid, bool isMUC) {
	if (jid.isBare() && !isMUC) {
		return presenceOracle_->getHighestPriorityPresence(jid);
	}

	std::vector<Presence::ref> mucPresence = presenceOracle_->getAllPresence(jid.toBare());

	if (isMUC && !mucPresence.empty()) {
		Presence::ref presence = boost::make_shared<Presence>(Presence());
		presence->setFrom(jid);
		return presence;
	}

	foreach (Presence::ref presence, mucPresence) {
		if (presence.get() && presence->getFrom() == jid) {
			return presence;
		}
	}

	return Presence::create();
}

}
