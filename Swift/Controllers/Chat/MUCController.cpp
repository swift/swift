/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Chat/MUCController.h>

#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/format.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Base/foreach.h>
#include <SwifTools/TabComplete.h>
#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/MUC/MUC.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/SetAvatar.h>
#include <Swift/Controllers/Roster/SetPresence.h>
#include <Swiften/Disco/EntityCapsProvider.h>


#define MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS 60000

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
MUCController::MUCController (
		const JID& self,
		MUC::ref muc,
		const boost::optional<std::string>& password,
		const std::string &nick, 
		StanzaChannel* stanzaChannel, 
		IQRouter* iqRouter, 
		ChatWindowFactory* chatWindowFactory, 
		PresenceOracle* presenceOracle,
		AvatarManager* avatarManager,
		UIEventStream* uiEventStream,
		bool useDelayForLatency,
		TimerFactory* timerFactory,
		EventController* eventController,
		EntityCapsProvider* entityCapsProvider) :
			ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, muc->getJID(), presenceOracle, avatarManager, useDelayForLatency, uiEventStream, eventController, timerFactory, entityCapsProvider), muc_(muc), nick_(nick), desiredNick_(nick), password_(password) {
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
	chatWindow_->onOccupantSelectionChanged.connect(boost::bind(&MUCController::handleWindowOccupantSelectionChanged, this, _1));
	chatWindow_->onOccupantActionSelected.connect(boost::bind(&MUCController::handleActionRequestedOnOccupant, this, _1, _2));
	chatWindow_->onChangeSubjectRequest.connect(boost::bind(&MUCController::handleChangeSubjectRequest, this, _1));
	chatWindow_->onConfigureRequest.connect(boost::bind(&MUCController::handleConfigureRequest, this, _1));
	chatWindow_->onConfigurationFormCancelled.connect(boost::bind(&MUCController::handleConfigurationCancelled, this));
	chatWindow_->onDestroyRequest.connect(boost::bind(&MUCController::handleDestroyRoomRequest, this));
	chatWindow_->onInvitePersonToThisMUCRequest.connect(boost::bind(&MUCController::handleInvitePersonToThisMUCRequest, this, _1, _2));
	chatWindow_->onGetAffiliationsRequest.connect(boost::bind(&MUCController::handleGetAffiliationsRequest, this));
	chatWindow_->onChangeAffiliationsRequest.connect(boost::bind(&MUCController::handleChangeAffiliationsRequest, this, _1));
	muc_->onJoinComplete.connect(boost::bind(&MUCController::handleJoinComplete, this, _1));
	muc_->onJoinFailed.connect(boost::bind(&MUCController::handleJoinFailed, this, _1));
	muc_->onOccupantJoined.connect(boost::bind(&MUCController::handleOccupantJoined, this, _1));
	muc_->onOccupantPresenceChange.connect(boost::bind(&MUCController::handleOccupantPresenceChange, this, _1));
	muc_->onOccupantLeft.connect(boost::bind(&MUCController::handleOccupantLeft, this, _1, _2, _3));
	muc_->onOccupantRoleChanged.connect(boost::bind(&MUCController::handleOccupantRoleChanged, this, _1, _2, _3));
	muc_->onConfigurationFailed.connect(boost::bind(&MUCController::handleConfigurationFailed, this, _1));
	muc_->onConfigurationFormReceived.connect(boost::bind(&MUCController::handleConfigurationFormReceived, this, _1));
	muc_->onRoleChangeFailed.connect(boost::bind(&MUCController::handleOccupantRoleChangeFailed, this, _1, _2, _3));
	muc_->onAffiliationListReceived.connect(boost::bind(&MUCController::handleAffiliationListReceived, this, _1, _2));
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
	handleBareJIDCapsChanged(muc->getJID());
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

void MUCController::handleWindowOccupantSelectionChanged(ContactRosterItem* item) {
	std::vector<ChatWindow::OccupantAction> actions;
	/* FIXME: all of these should be conditional */
	if (item) {
		actions.push_back(ChatWindow::Kick);
		actions.push_back(ChatWindow::Ban);
		actions.push_back(ChatWindow::MakeModerator);
		actions.push_back(ChatWindow::MakeParticipant);
		actions.push_back(ChatWindow::MakeVisitor);
	}
	chatWindow_->setAvailableOccupantActions(actions);
}

void MUCController::handleActionRequestedOnOccupant(ChatWindow::OccupantAction action, ContactRosterItem* item) {
	JID mucJID = item->getJID();
	MUCOccupant occupant = muc_->getOccupant(mucJID.getResource());
	JID realJID;
	if (occupant.getRealJID()) {
		realJID = occupant.getRealJID().get();
	}
	switch (action) {
		case ChatWindow::Kick: muc_->kickOccupant(mucJID);break;
		case ChatWindow::Ban: muc_->changeAffiliation(realJID, MUCOccupant::Outcast);break;
		case ChatWindow::MakeModerator: muc_->changeOccupantRole(mucJID, MUCOccupant::Moderator);break;
		case ChatWindow::MakeParticipant: muc_->changeOccupantRole(mucJID, MUCOccupant::Participant);break;
		case ChatWindow::MakeVisitor: muc_->changeOccupantRole(mucJID, MUCOccupant::Visitor);break;
	}
}

void MUCController::handleBareJIDCapsChanged(const JID& /*jid*/) {
	ChatWindow::Tristate support = ChatWindow::Yes;
	bool any = false;
	foreach (const std::string& nick, currentOccupants_) {
		DiscoInfo::ref disco = entityCapsProvider_->getCaps(toJID_.toBare().toString() + "/" + nick);
		if (disco && disco->hasFeature(DiscoInfo::MessageCorrectionFeature)) {
			any = true;
		} else {
			support = ChatWindow::Maybe;
		}
	}
	if (!any) {
		support = ChatWindow::No;
	}
	chatWindow_->setCorrectionEnabled(support);
}

/**
 * Join the MUC if not already in it.
 */
void MUCController::rejoin() {
	if (parting_) {
		joined_ = false;
		parting_ = false;
		if (password_) {
			muc_->setPassword(*password_);
		}
		//FIXME: check for received activity
		if (lastActivity_ == boost::posix_time::not_a_date_time) {
			muc_->joinAs(nick_);
		} else {
			muc_->joinWithContextSince(nick_, lastActivity_);
		}
	}
}

bool MUCController::isJoined() {
	return joined_;
}

const std::string& MUCController::getNick() {
	return nick_;
}

void MUCController::handleJoinTimeoutTick() {
	receivedActivity();
	chatWindow_->addSystemMessage(str(format(QT_TRANSLATE_NOOP("", "Room %1% is not responding. This operation may never complete.")) % toJID_.toString()));
}

void MUCController::receivedActivity() {
	if (loginCheckTimer_) {
		loginCheckTimer_->stop();
	}
}

void MUCController::handleJoinFailed(boost::shared_ptr<ErrorPayload> error) {
	receivedActivity();
	std::string errorMessage = QT_TRANSLATE_NOOP("", "Unable to enter this room");
	std::string rejoinNick;
	if (error) {
		switch (error->getCondition()) {
		case ErrorPayload::Conflict:
			rejoinNick = nick_ + "_";
			errorMessage = str(format(QT_TRANSLATE_NOOP("", "Unable to enter this room as %1%, retrying as %2%")) % nick_ % rejoinNick);
			break;
		case ErrorPayload::JIDMalformed: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "No nickname specified");
			break;
		case ErrorPayload::NotAuthorized: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "The correct room password is needed");
			break;
		case ErrorPayload::RegistrationRequired: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "Only members may enter"); 
			break;
		case ErrorPayload::Forbidden: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "You are banned from the room"); 
			break;
		case ErrorPayload::ServiceUnavailable: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "The room is full");
			break;
		case ErrorPayload::ItemNotFound: 
			errorMessage += ": ";
			errorMessage += QT_TRANSLATE_NOOP("", "The room does not exist");
			break;
			
		default: break;
		}
	}
	errorMessage = str(format(QT_TRANSLATE_NOOP("", "Couldn't join room: %1%.")) % errorMessage);
	chatWindow_->addErrorMessage(errorMessage);
	parting_ = true;
	if (!rejoinNick.empty()) {
		nick_ = rejoinNick;
		rejoin();
	}
}

void MUCController::handleJoinComplete(const std::string& nick) {
	receivedActivity();
	joined_ = true;
	std::string joinMessage = str(format(QT_TRANSLATE_NOOP("", "You have entered room %1% as %2%.")) % toJID_.toString() % nick);
	nick_ = nick;
	chatWindow_->addSystemMessage(joinMessage);
	clearPresenceQueue();
	shouldJoinOnReconnect_ = true;
	setEnabled(true);
	onUserJoined();
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
	std::string groupName(roleToGroupName(occupant.getRole()));
	roster_->addContact(jid, realJID, occupant.getNick(), groupName, avatarManager_->getAvatarPath(jid).string());
	roster_->getGroup(groupName)->setManualSort(roleToSortName(occupant.getRole()));
	if (joined_) {
		std::string joinString;
		MUCOccupant::Role role = occupant.getRole();
		if (role != MUCOccupant::NoRole && role != MUCOccupant::Participant) {
			joinString = str(format(QT_TRANSLATE_NOOP("", "%1% has entered the room as a %2%.")) % occupant.getNick() % roleToFriendlyName(role));
		}
		else {
			joinString = str(format(QT_TRANSLATE_NOOP("", "%1% has entered the room.")) % occupant.getNick());
		}
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
	case MUCOccupant::Moderator: return QT_TRANSLATE_NOOP("", "moderator");
	case MUCOccupant::Participant: return QT_TRANSLATE_NOOP("", "participant");
	case MUCOccupant::Visitor: return QT_TRANSLATE_NOOP("", "visitor");
	case MUCOccupant::NoRole: return "";
	}
	return "";
}

std::string MUCController::roleToSortName(MUCOccupant::Role role) {
	switch (role) {
	case MUCOccupant::Moderator: return "1";
	case MUCOccupant::Participant: return "2";
	case MUCOccupant::Visitor: return "3";
	case MUCOccupant::NoRole: return "4";
	}
	return "5";
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

	if (message->hasSubject() && message->getBody().empty()) {
		chatWindow_->addSystemMessage(str(format(QT_TRANSLATE_NOOP("", "The room subject is now: %1%")) % message->getSubject()));;
		chatWindow_->setSubject(message->getSubject());
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_ && !message->getPayload<Delay>()) {
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_) {
		messageEvent->conclude();
	}
}

void MUCController::postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	if (joined_ && messageEvent->getStanza()->getFrom().getResource() != nick_ && messageTargetsMe(message) && !message->getPayload<Delay>()) {
		eventController_->handleIncomingEvent(messageEvent);
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
	std::string group(roleToGroupName(occupant.getRole()));
	roster_->addContact(jid, realJID, nick, group, avatarManager_->getAvatarPath(jid).string());
	roster_->getGroup(group)->setManualSort(roleToSortName(occupant.getRole()));
	chatWindow_->addSystemMessage(str(format(QT_TRANSLATE_NOOP("", "%1% is now a %2%")) % nick % roleToFriendlyName(occupant.getRole())));
}

std::string MUCController::roleToGroupName(MUCOccupant::Role role) {
	std::string result;
	switch (role) {
	case MUCOccupant::Moderator: result = QT_TRANSLATE_NOOP("", "Moderators"); break;
	case MUCOccupant::Participant: result = QT_TRANSLATE_NOOP("", "Participants"); break;
	case MUCOccupant::Visitor: result = QT_TRANSLATE_NOOP("", "Visitors"); break;
	case MUCOccupant::NoRole: result = QT_TRANSLATE_NOOP("", "Occupants"); break;
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
			chatWindow_->addSystemMessage(str(format(QT_TRANSLATE_NOOP("", "Trying to enter room %1%")) % toJID_.toString()));
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

void MUCController::handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const std::string& reason) {
	NickJoinPart event(occupant.getNick(), Part);
	appendToJoinParts(joinParts_, event);
	currentOccupants_.erase(occupant.getNick());
	completer_->removeWord(occupant.getNick());
	std::string partMessage;
	bool clearAfter = false;
	if (occupant.getNick() != nick_) {
		std::string partType;
		switch (type) {
			case MUC::LeaveKick: clearPresenceQueue(); clearAfter = true; partType = " (kicked)"; break;
			case MUC::LeaveBan: clearPresenceQueue(); clearAfter = true; partType = " (banned)"; break;
			case MUC::LeaveNotMember: clearPresenceQueue(); clearAfter = true; partType = " (no longer a member)"; break;
			case MUC::LeaveDestroy:
			case MUC::Disconnect:
			case MUC::LeavePart: break;
		}
		partMessage = str(format(QT_TRANSLATE_NOOP("", "%1% has left the room%2%")) % occupant.getNick() % partType);
	}
	else {
		switch (type) {
			case MUC::LeaveKick: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "You have been kicked out of the room"); break;
			case MUC::LeaveBan: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "You have been banned from the room"); break;
			case MUC::LeaveNotMember: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "You are no longer a member of the room and have been removed"); break;
			case MUC::LeaveDestroy: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "The room has been destroyed"); break;
			case MUC::Disconnect:
			case MUC::LeavePart: partMessage = QT_TRANSLATE_NOOP("", "You have left the room");
		}
	}
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
	if (clearAfter) {
		clearPresenceQueue();
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
	for (; it != joinParts.end(); ++it) {
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
				result += QT_TRANSLATE_NOOP("", " and ");
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
		std::string names = concatenateListOfNames(sorted[i]);
		if (!names.empty()) {
			std::string eventString;
			switch (i) {
				case Join: 
					if (sorted[i].size() > 1) {
						eventString = QT_TRANSLATE_NOOP("", "%1% have entered the room");
					}
					else {
						eventString = QT_TRANSLATE_NOOP("", "%1% has entered the room");
					}
					break;
				case Part: 
					if (sorted[i].size() > 1) {
						eventString = QT_TRANSLATE_NOOP("", "%1% have left the room");
					}
					else {
						eventString = QT_TRANSLATE_NOOP("", "%1% has left the room");
					}
					break;
				case JoinThenPart: 
					if (sorted[i].size() > 1) {
						eventString = QT_TRANSLATE_NOOP("", "%1% have entered then left the room");
					}
					else {
						eventString = QT_TRANSLATE_NOOP("", "%1% has entered then left the room");
					}
					break;
				case PartThenJoin: 
					if (sorted[i].size() > 1) {
						eventString = QT_TRANSLATE_NOOP("", "%1% have left then returned to the room");
					}
					else {
						eventString = QT_TRANSLATE_NOOP("", "%1% has left then returned to the room");
					}
					break;
			}
			populatedEvents.push_back(static_cast<JoinPart>(i));
			eventStrings[i] = str(boost::format(eventString) % names);
		}
	}
	for (size_t i = 0; i < populatedEvents.size(); i++) {
		if (i > 0) {
			if (i < populatedEvents.size() - 1) {
				result += ", ";
			} else {
				result += QT_TRANSLATE_NOOP("", " and ");
			}
		}
		result += eventStrings[populatedEvents[i]];
	}
	return result;
}

void MUCController::handleChangeSubjectRequest(const std::string& subject) {
	muc_->changeSubject(subject);
}

void MUCController::handleConfigureRequest(Form::ref form) {
	if (form) {
		muc_->configureRoom(form);
	}
	else {
		muc_->requestConfigurationForm();
	}
}

void MUCController::handleConfigurationFailed(ErrorPayload::ref error) {
	std::string errorMessage = getErrorMessage(error);
	errorMessage = str(format(QT_TRANSLATE_NOOP("", "Room configuration failed: %1%.")) % errorMessage);
	chatWindow_->addErrorMessage(errorMessage);
}

void MUCController::handleOccupantRoleChangeFailed(ErrorPayload::ref error, const JID&, MUCOccupant::Role) {
	std::string errorMessage = getErrorMessage(error);
	errorMessage = str(format(QT_TRANSLATE_NOOP("", "Occupant role change failed: %1%.")) % errorMessage);
	chatWindow_->addErrorMessage(errorMessage);
}

void MUCController::handleConfigurationFormReceived(Form::ref form) {
	chatWindow_->showRoomConfigurationForm(form);
}

void MUCController::handleConfigurationCancelled() {
	muc_->cancelConfigureRoom();
}

void MUCController::handleDestroyRoomRequest() {
	muc_->destroyRoom();
}

void MUCController::handleInvitePersonToThisMUCRequest(const JID& jid, const std::string& reason) {
	muc_->invitePerson(jid, reason);
}

void MUCController::handleGetAffiliationsRequest() {
	muc_->requestAffiliationList(MUCOccupant::Owner);
	muc_->requestAffiliationList(MUCOccupant::Admin);
	muc_->requestAffiliationList(MUCOccupant::Member);
	muc_->requestAffiliationList(MUCOccupant::Outcast);
}

typedef std::pair<MUCOccupant::Affiliation, JID> AffiliationChangePair;

void MUCController::handleChangeAffiliationsRequest(const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& changes) {
	std::set<JID> addedJIDs;
	foreach (const AffiliationChangePair& change, changes) {
		if (change.first != MUCOccupant::NoAffiliation) {
			addedJIDs.insert(change.second);
		}
	}
	foreach (const AffiliationChangePair& change, changes) {
		if (change.first != MUCOccupant::NoAffiliation || addedJIDs.find(change.second) == addedJIDs.end()) {
			muc_->changeAffiliation(change.second, change.first);
		}
	}
}

void MUCController::handleAffiliationListReceived(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
	chatWindow_->setAffiliations(affiliation, jids);
}

}
