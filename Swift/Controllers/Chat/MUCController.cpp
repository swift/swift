/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/MUCController.h"

#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "Swiften/Network/Timer.h"
#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Base/foreach.h"
#include "SwifTools/TabComplete.h"
#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Elements/Delay.h"
#include "Swiften/MUC/MUC.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Roster/SetAvatar.h"
#include "Swift/Controllers/Roster/SetPresence.h"


#define MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS 60000

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
MUCController::MUCController (
		const JID& self,
		MUC::ref muc,
		const std::string &nick, 
		StanzaChannel* stanzaChannel, 
		IQRouter* iqRouter, 
		ChatWindowFactory* chatWindowFactory, 
		PresenceOracle* presenceOracle,
		AvatarManager* avatarManager,
		UIEventStream* uiEventStream,
		bool useDelayForLatency,
		TimerFactory* timerFactory,
		EventController* eventController) :
			ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, muc->getJID(), presenceOracle, avatarManager, useDelayForLatency, uiEventStream, eventController, timerFactory), muc_(muc), nick_(nick), desiredNick_(nick) {
	parting_ = true;
	joined_ = false;
	lastWasPresence_ = false;
	shouldJoinOnReconnect_ = true;
	doneGettingHistory_ = false;
	events_ = uiEventStream;
	
	roster_ = new Roster(false, true);
	completer_ = new TabComplete();
	chatWindow_->setRosterModel(roster_);
	chatWindow_->setTabComplete(completer_);
	chatWindow_->setName(muc->getJID().getNode());
	chatWindow_->onClosed.connect(boost::bind(&MUCController::handleWindowClosed, this));
	muc_->onJoinComplete.connect(boost::bind(&MUCController::handleJoinComplete, this, _1));
	muc_->onJoinFailed.connect(boost::bind(&MUCController::handleJoinFailed, this, _1));
	muc_->onOccupantJoined.connect(boost::bind(&MUCController::handleOccupantJoined, this, _1));
	muc_->onOccupantPresenceChange.connect(boost::bind(&MUCController::handleOccupantPresenceChange, this, _1));
	muc_->onOccupantLeft.connect(boost::bind(&MUCController::handleOccupantLeft, this, _1, _2, _3));
	muc_->onOccupantRoleChanged.connect(boost::bind(&MUCController::handleOccupantRoleChanged, this, _1, _2, _3));
	if (timerFactory) {
		loginCheckTimer_ = boost::shared_ptr<Timer>(timerFactory->createTimer(MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS));
		loginCheckTimer_->onTick.connect(boost::bind(&MUCController::handleJoinTimeoutTick, this));
		loginCheckTimer_->start();
	}
	chatWindow_->convertToMUC();
	setOnline(true);
	if (avatarManager_ != NULL) {
		avatarChangedConnection_ = (avatarManager_->onAvatarChanged.connect(boost::bind(&MUCController::handleAvatarChanged, this, _1)));
	} 
}

MUCController::~MUCController() {
	chatWindow_->setRosterModel(NULL);
	delete roster_;
	if (loginCheckTimer_) {
		loginCheckTimer_->stop();
	}
	chatWindow_->setTabComplete(NULL);
	delete completer_;
}

/**
 * Join the MUC if not already in it.
 */
void MUCController::rejoin() {
	if (parting_) {
		joined_ = false;
		parting_ = false;
		//FIXME: check for received activity
		if (lastActivity_ == boost::posix_time::not_a_date_time) {
			muc_->joinAs(nick_);
		} else {
			muc_->joinWithContextSince(nick_, lastActivity_);
		}
	}
}

void MUCController::handleJoinTimeoutTick() {
	receivedActivity();
	chatWindow_->addSystemMessage("Room " + toJID_.toString() + " is not responding. This operation may never complete");
}

void MUCController::receivedActivity() {
	if (loginCheckTimer_) {
		loginCheckTimer_->stop();
	}
}

void MUCController::handleJoinFailed(boost::shared_ptr<ErrorPayload> error) {
	receivedActivity();
	std::string errorMessage = "Unable to join this room";
	std::string rejoinNick;
	if (error) {
		switch (error->getCondition()) {
		case ErrorPayload::Conflict: rejoinNick = nick_ + "_"; errorMessage += " as " + nick_ + ", retrying as " + rejoinNick; break;
		case ErrorPayload::JIDMalformed: errorMessage += ", no nickname specified";break;
		case ErrorPayload::NotAuthorized: errorMessage += ", a password needed";break;
		case ErrorPayload::RegistrationRequired: errorMessage += ", only members may join"; break;
		case ErrorPayload::Forbidden: errorMessage += ", you are banned from the room"; break;
		case ErrorPayload::ServiceUnavailable: errorMessage += ", the room is full";break;
		case ErrorPayload::ItemNotFound: errorMessage += ", the room does not exist";break;
			
		default: break;
		}
	}
	errorMessage += ".";
	chatWindow_->addErrorMessage(errorMessage);
	if (!rejoinNick.empty()) {
		nick_ = rejoinNick;
		parting_ = true;
		rejoin();
	}
}

void MUCController::handleJoinComplete(const std::string& nick) {
	receivedActivity();
	joined_ = true;
	std::string joinMessage = "You have joined room " + toJID_.toString() + " as " + nick;
	nick_ = nick;
	chatWindow_->addSystemMessage(joinMessage);
	clearPresenceQueue();
	shouldJoinOnReconnect_ = true;
	setEnabled(true);
}

void MUCController::handleAvatarChanged(const JID& jid) {
	if (parting_ || !jid.equals(toJID_, JID::WithoutResource)) {
		return;
	}
	std::string path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path, JID::WithResource));
}

void MUCController::handleWindowClosed() {
	parting_ = true;
	shouldJoinOnReconnect_ = false;
	muc_->part();
	onUserLeft();
}

void MUCController::handleOccupantJoined(const MUCOccupant& occupant) {
	if (nick_ != occupant.getNick()) {
		completer_->addWord(occupant.getNick());
	}
	receivedActivity();
	JID jid(nickToJID(occupant.getNick()));
	JID realJID;
	if (occupant.getRealJID()) {
		realJID = occupant.getRealJID().get();
	}
	currentOccupants_.insert(occupant.getNick());
	NickJoinPart event(occupant.getNick(), Join);
	appendToJoinParts(joinParts_, event);
	roster_->addContact(jid, realJID, occupant.getNick(), roleToGroupName(occupant.getRole()), avatarManager_->getAvatarPath(jid).string());
	if (joined_) {
		std::string joinString = occupant.getNick() + " has joined the room";
		MUCOccupant::Role role = occupant.getRole();
		if (role != MUCOccupant::NoRole && role != MUCOccupant::Participant) {
			joinString += " as a " + roleToFriendlyName(role);

		}
		joinString += ".";
		if (shouldUpdateJoinParts()) {
			updateJoinParts();
		} else {
			addPresenceMessage(joinString);

		}
	}
	if (avatarManager_ != NULL) {
		handleAvatarChanged(jid);
	}
}

void MUCController::addPresenceMessage(const std::string& message) {
	lastWasPresence_ = true;
	chatWindow_->addPresenceMessage(message);
}

void MUCController::clearPresenceQueue() {
	lastWasPresence_ = false;
	joinParts_.clear();
}

std::string MUCController::roleToFriendlyName(MUCOccupant::Role role) {
	switch (role) {
	case MUCOccupant::Moderator: return "moderator";
	case MUCOccupant::Participant: return "participant";
	case MUCOccupant::Visitor: return "visitor";
	case MUCOccupant::NoRole: return "";
	}
	return "";
}

JID MUCController::nickToJID(const std::string& nick) {
	return JID(toJID_.getNode(), toJID_.getDomain(), nick);
}

bool MUCController::messageTargetsMe(boost::shared_ptr<Message> message) {
	std::string stringRegexp(".*\\b" + boost::to_lower_copy(nick_) + "\\b.*");
	boost::regex myRegexp(stringRegexp);
	return boost::regex_match(boost::to_lower_copy(message->getBody()), myRegexp);
}

void MUCController::preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	if (messageEvent->getStanza()->getType() == Message::Groupchat) {
		lastActivity_ = boost::posix_time::microsec_clock::universal_time();
	}
	clearPresenceQueue();
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	if (joined_ && messageEvent->getStanza()->getFrom().getResource() != nick_ && messageTargetsMe(message) && !message->getPayload<Delay>()) {
		eventController_->handleIncomingEvent(messageEvent);
		if (messageEvent->isReadable()) {
			chatWindow_->flash();
		}
	}
	if (joined_) {
		std::string nick = message->getFrom().getResource();
		if (nick != nick_ && currentOccupants_.find(nick) != currentOccupants_.end()) {
			completer_->addWord(nick);
		}
	}
	/*Buggy implementations never send the status code, so use an incoming message as a hint that joining's done (e.g. the old ejabberd on psi-im.org).*/
	receivedActivity();
	joined_ = true;

	if (!message->getSubject().empty() && message->getBody().empty()) {
		chatWindow_->addSystemMessage("The room subject is now: " + message->getSubject());
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_ && !message->getPayload<Delay>()) {
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_) {
		messageEvent->conclude();
	}
}

void MUCController::handleOccupantRoleChanged(const std::string& nick, const MUCOccupant& occupant, const MUCOccupant::Role& oldRole) {
	clearPresenceQueue();
	receivedActivity();
	JID jid(nickToJID(nick));
	roster_->removeContactFromGroup(jid, roleToGroupName(oldRole));
	JID realJID;
	if (occupant.getRealJID()) {
		realJID = occupant.getRealJID().get();
	}
	roster_->addContact(jid, realJID, nick, roleToGroupName(occupant.getRole()), avatarManager_->getAvatarPath(jid).string());
	chatWindow_->addSystemMessage(nick + " is now a " + roleToFriendlyName(occupant.getRole()));
}

std::string MUCController::roleToGroupName(MUCOccupant::Role role) {
	std::string result;
	switch (role) {
	case MUCOccupant::Moderator: result = "Moderators"; break;
	case MUCOccupant::Participant: result = "Participants"; break;
	case MUCOccupant::Visitor: result = "Visitors"; break;
	case MUCOccupant::NoRole: result = "Occupants"; break;
	default: assert(false);
	}
	return result;
}

void MUCController::setOnline(bool online) {
	ChatControllerBase::setOnline(online);
	if (!online) {
		muc_->part();
		parting_ = true;
		processUserPart();
	} else {
		if (shouldJoinOnReconnect_) {
			chatWindow_->addSystemMessage("Trying to join room " + toJID_.toString());
			if (loginCheckTimer_) {
				loginCheckTimer_->start();
			}
			nick_ = desiredNick_;
			rejoin();
		}
	}
}

void MUCController::processUserPart() {
	roster_->removeAll();
	/* handleUserLeft won't throw a part back up unless this is called
	   when it doesn't yet know we've left - which only happens on
	   disconnect, so call with disconnect here so if the signal does
	   bubble back up, it'll be with the right type.*/
	muc_->handleUserLeft(MUC::Disconnect);
	setEnabled(false);
}

bool MUCController::shouldUpdateJoinParts() {
	return lastWasPresence_;
}

void MUCController::handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType, const std::string& reason) {
	NickJoinPart event(occupant.getNick(), Part);
	appendToJoinParts(joinParts_, event);
	currentOccupants_.erase(occupant.getNick());
	completer_->removeWord(occupant.getNick());
	std::string partMessage = (occupant.getNick() != nick_) ? occupant.getNick() + " has left the room" : "You have left the room";
	if (!reason.empty()) {
		partMessage += " (" + reason + ")";
	}
	partMessage += ".";

	if (occupant.getNick() != nick_) {
		if (shouldUpdateJoinParts()) {
			updateJoinParts();
		} else {
			addPresenceMessage(partMessage);
		}
		roster_->removeContact(JID(toJID_.getNode(), toJID_.getDomain(), occupant.getNick()));
	} else {
		addPresenceMessage(partMessage);
		parting_ = true;
		processUserPart();
	}
}

void MUCController::handleOccupantPresenceChange(boost::shared_ptr<Presence> presence) {
	receivedActivity();
	roster_->applyOnItems(SetPresence(presence, JID::WithResource));
}

bool MUCController::isIncomingMessageFromMe(boost::shared_ptr<Message> message) {
	JID from = message->getFrom();
	return nick_ == from.getResource();
}

std::string MUCController::senderDisplayNameFromMessage(const JID& from) {
	return from.getResource();
}

void MUCController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	message->setType(Swift::Message::Groupchat);
}

boost::optional<boost::posix_time::ptime> MUCController::getMessageTimestamp(boost::shared_ptr<Message> message) const {
	return message->getTimestampFrom(toJID_);
}

void MUCController::updateJoinParts() {
	chatWindow_->replaceLastMessage(generateJoinPartString(joinParts_));
}

void MUCController::appendToJoinParts(std::vector<NickJoinPart>& joinParts, const NickJoinPart& newEvent) {
	std::vector<NickJoinPart>::iterator it = joinParts.begin();
	bool matched = false;
	for (; it != joinParts.end(); it++) {
		if ((*it).nick == newEvent.nick) {
			matched = true;
			JoinPart type = (*it).type;
			switch (newEvent.type) {
				case Join: type = (type == Part) ? PartThenJoin : Join; break;
				case Part: type = (type == Join) ? JoinThenPart : Part; break;
				default: /*Nothing to see here */;break;
			}
			(*it).type = type;
			break;
		}
	}
	if (!matched) {
		joinParts.push_back(newEvent);
	}
}

std::string MUCController::concatenateListOfNames(const std::vector<NickJoinPart>& joinParts) {
	std::string result;
	for (size_t i = 0; i < joinParts.size(); i++) {
		if (i > 0) {
			if (i < joinParts.size() - 1) {
				result += ", ";
			} else {
				result += " and ";
			}
		}
		NickJoinPart event = joinParts[i];
		result += event.nick;
	}
	return result;
}

std::string MUCController::generateJoinPartString(const std::vector<NickJoinPart>& joinParts) {
	std::vector<NickJoinPart> sorted[4];
	std::string eventStrings[4];
	foreach (NickJoinPart event, joinParts) {
		sorted[event.type].push_back(event);
	}
	std::string result;
	std::vector<JoinPart> populatedEvents;
	for (size_t i = 0; i < 4; i++) {
		std::string eventString = concatenateListOfNames(sorted[i]);
		if (!eventString.empty()) {
			std::string haveHas = sorted[i].size() > 1 ? " have" : " has";
			switch (i) {
				case Join: eventString += haveHas + " joined";break;
				case Part: eventString += haveHas + " left";break;
				case JoinThenPart: eventString += " joined then left";break;
				case PartThenJoin: eventString += " left then rejoined";break;
			}
			populatedEvents.push_back(static_cast<JoinPart>(i));
			eventStrings[i] = eventString;
		}
	}
	for (size_t i = 0; i < populatedEvents.size(); i++) {
		if (i > 0) {
			if (i < populatedEvents.size() - 1) {
				result += ", ";
			} else {
				result += " and ";
			}
		}
		result += eventStrings[populatedEvents[i]];
	}
	result += " the room.";
	return result;
}

}
