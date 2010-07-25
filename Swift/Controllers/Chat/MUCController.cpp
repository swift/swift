/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/MUCController.h"

#include <boost/bind.hpp>

#include "Swiften/Network/Timer.h"
#include "Swiften/Network/TimerFactory.h"
#include "SwifTools/TabComplete.h"
#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Elements/Delay.h"
#include "Swiften/MUC/MUC.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Roster/Roster.h"
#include "Swiften/Roster/SetAvatar.h"
#include "Swiften/Roster/SetPresence.h"


#define MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS 60000

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
MUCController::MUCController (
		const JID& self,
		const JID &muc, 
		const String &nick, 
		StanzaChannel* stanzaChannel, 
		PresenceSender* presenceSender,
		IQRouter* iqRouter, 
		ChatWindowFactory* chatWindowFactory, 
		PresenceOracle* presenceOracle,
		AvatarManager* avatarManager,
		UIEventStream* uiEventStream,
		bool useDelayForLatency,
		TimerFactory* timerFactory,
		EventController* eventController) :
			ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, muc, presenceOracle, avatarManager, useDelayForLatency, uiEventStream, eventController), muc_(new MUC(stanzaChannel, presenceSender, muc)), nick_(nick) {
	parting_ = true;
	joined_ = false;
	events_ = uiEventStream;
	
	roster_ = new Roster(true);
	completer_ = new TabComplete();
	chatWindow_->setRosterModel(roster_);
	chatWindow_->setTabComplete(completer_);
	chatWindow_->setName(muc.getNode());
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
	chatWindow_->addSystemMessage("Trying to join room " + toJID_.toString());
	rejoin();
	if (avatarManager_ != NULL) {
		avatarChangedConnection_ = (avatarManager_->onAvatarChanged.connect(boost::bind(&MUCController::handleAvatarChanged, this, _1, _2)));
	} 
}

MUCController::~MUCController() {
	delete muc_;
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
		muc_->joinAs(nick_);
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
	String errorMessage = "Unable to join this room";
	String rejoinNick;
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
	if (!rejoinNick.isEmpty()) {
		nick_ = rejoinNick;
		parting_ = true;
		rejoin();
	}
}

void MUCController::handleJoinComplete(const String& nick) {
	receivedActivity();
	joined_ = true;
	String joinMessage = "You have joined room " + toJID_.toString() + " as " + nick;
	nick_ = nick;
	chatWindow_->addSystemMessage(joinMessage);
	setEnabled(true);
}

void MUCController::handleAvatarChanged(const JID& jid, const String&) {
	if (parting_ || !jid.equals(toJID_, JID::WithoutResource)) {
		return;
	}
	String path = avatarManager_->getAvatarPath(jid).string();
	roster_->applyOnItems(SetAvatar(jid, path, JID::WithResource));
}

void MUCController::handleWindowClosed() {
	parting_ = true;
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
	roster_->addContact(jid, realJID, occupant.getNick(), roleToGroupName(occupant.getRole()));
	if (joined_) {
		String joinString = occupant.getNick() + " has joined the room";
		MUCOccupant::Role role = occupant.getRole();
		if (role != MUCOccupant::NoRole && role != MUCOccupant::Participant) {
			joinString += " as a " + roleToFriendlyName(role);

		}
		joinString += ".";
		chatWindow_->addSystemMessage(joinString);
	}
	if (avatarManager_ != NULL) {
		handleAvatarChanged(jid, "dummy");
	}
}

String MUCController::roleToFriendlyName(MUCOccupant::Role role) {
	switch (role) {
	case MUCOccupant::Moderator: return "moderator";
	case MUCOccupant::Participant: return "participant";
	case MUCOccupant::Visitor: return "visitor";
	case MUCOccupant::NoRole: return "";
	}
	return "";
}

JID MUCController::nickToJID(const String& nick) {
	return JID(toJID_.getNode(), toJID_.getDomain(), nick);
}

bool MUCController::messageTargetsMe(boost::shared_ptr<Message> message) {
	return message->getBody().contains(nick_);
}

void MUCController::preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	if (joined_ && messageTargetsMe(message) && !message->getPayload<Delay>()) {
		eventController_->handleIncomingEvent(messageEvent);
	}
	if (joined_) {
		String nick = message->getFrom().getResource();
		if (nick != nick_) {
			completer_->addWord(nick);
		}
	}
	/*Buggy implementations never send the status code, so use an incoming message as a hint that joining's done (e.g. the old ejabberd on psi-im.org).*/
	receivedActivity();
	joined_ = true;

	if (!message->getSubject().isEmpty() && message->getBody().isEmpty()) {
		chatWindow_->addSystemMessage("The room subject is now: " + message->getSubject());
	}
}

void MUCController::handleOccupantRoleChanged(const String& nick, const MUCOccupant& occupant, const MUCOccupant::Role& oldRole) {
	receivedActivity();
	JID jid(nickToJID(nick));
	roster_->removeContactFromGroup(jid, roleToGroupName(oldRole));
	JID realJID;
	if (occupant.getRealJID()) {
		realJID = occupant.getRealJID().get();
	}
	roster_->addContact(jid, realJID, nick, roleToGroupName(occupant.getRole()));
	chatWindow_->addSystemMessage(nick + " is now a " + roleToFriendlyName(occupant.getRole()));
}

String MUCController::roleToGroupName(MUCOccupant::Role role) {
	String result;
	switch (role) {
	case MUCOccupant::Moderator: result = "Moderators"; break;
	case MUCOccupant::Participant: result = "Participants"; break;
	case MUCOccupant::Visitor: result = "Visitors"; break;
	case MUCOccupant::NoRole: result = "Occupants"; break;
	default: assert(false);
	}
	return result;
}

void MUCController::setEnabled(bool enabled) {
	ChatControllerBase::setEnabled(enabled);
	if (!enabled) {
		roster_->removeAll();
		/* handleUserLeft won't throw a part back up unless this is called
		   when it doesn't yet know we've left - which only happens on
		   disconnect, so call with disconnect here so if the signal does
		   bubble back up, it'll be with the right type.*/
		muc_->handleUserLeft(MUC::Disconnect);
	}
}

void MUCController::handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType, const String& reason) {
	completer_->removeWord(occupant.getNick());
	String partMessage = (occupant.getNick() != nick_) ? occupant.getNick() + " has left the room" : "You have left the room";
	if (!reason.isEmpty()) {
		partMessage += " (" + reason + ")";
	}
	partMessage += ".";
	chatWindow_->addSystemMessage(partMessage);
	if (occupant.getNick() != nick_) {
		roster_->removeContact(JID(toJID_.getNode(), toJID_.getDomain(), occupant.getNick()));
	} else {
		parting_ = true;
		setEnabled(false);
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

String MUCController::senderDisplayNameFromMessage(const JID& from) {
	return from.getResource();
}

void MUCController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	message->setType(Swift::Message::Groupchat);
}

boost::optional<boost::posix_time::ptime> MUCController::getMessageTimestamp(boost::shared_ptr<Message> message) const {
	return message->getTimestampFrom(toJID_);
}

}
