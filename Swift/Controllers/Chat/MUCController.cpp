/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/MUCController.h>

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/format.h>
#include <Swiften/Base/Tristate.h>
#include <Swiften/Client/BlockList.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/MUC/MUC.h>
#include <Swiften/MUC/MUCBookmark.h>
#include <Swiften/MUC/MUCBookmarkManager.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Roster/XMPPRoster.h>

#include <SwifTools/TabComplete.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Highlighter.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/SetAvatar.h>
#include <Swift/Controllers/Roster/ItemOperations/SetMUC.h>
#include <Swift/Controllers/Roster/ItemOperations/SetPresence.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/RosterVCardProvider.h>
#include <Swift/Controllers/UIEvents/InviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestAddUserDialogUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChangeBlockStateUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestInviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/ShowProfileForRosterItemUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

#define MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS 60000

namespace Swift {

class MUCBookmarkPredicate {
	public:
		MUCBookmarkPredicate(const JID& mucJID) : roomJID_(mucJID) { }
		bool operator()(const MUCBookmark& operand) {
			return operand.getRoom() == roomJID_;
		}

	private:
		JID roomJID_;
};

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
		EntityCapsProvider* entityCapsProvider,
		XMPPRoster* roster,
		HistoryController* historyController,
		MUCRegistry* mucRegistry,
		HighlightManager* highlightManager,
		ClientBlockListManager* clientBlockListManager,
		boost::shared_ptr<ChatMessageParser> chatMessageParser,
		bool isImpromptu,
		AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider,
		VCardManager* vcardManager,
		MUCBookmarkManager* mucBookmarkManager) :
	ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, muc->getJID(), presenceOracle, avatarManager, useDelayForLatency, uiEventStream, eventController, timerFactory, entityCapsProvider, historyController, mucRegistry, highlightManager, chatMessageParser, autoAcceptMUCInviteDecider), muc_(muc), nick_(nick), desiredNick_(nick), password_(password), renameCounter_(0), isImpromptu_(isImpromptu), isImpromptuAlreadyConfigured_(false), clientBlockListManager_(clientBlockListManager), mucBookmarkManager_(mucBookmarkManager) {
	parting_ = true;
	joined_ = false;
	lastWasPresence_ = false;
	shouldJoinOnReconnect_ = true;
	doneGettingHistory_ = false;
	events_ = uiEventStream;
	xmppRoster_ = roster;
	
	roster_ = new Roster(false, true);
	rosterVCardProvider_ = new RosterVCardProvider(roster_, vcardManager, JID::WithResource);
	completer_ = new TabComplete();
	chatWindow_->setRosterModel(roster_);
	chatWindow_->setTabComplete(completer_);
	chatWindow_->onClosed.connect(boost::bind(&MUCController::handleWindowClosed, this));
	chatWindow_->onOccupantSelectionChanged.connect(boost::bind(&MUCController::handleWindowOccupantSelectionChanged, this, _1));
	chatWindow_->onOccupantActionSelected.connect(boost::bind(&MUCController::handleActionRequestedOnOccupant, this, _1, _2));
	chatWindow_->onChangeSubjectRequest.connect(boost::bind(&MUCController::handleChangeSubjectRequest, this, _1));
	chatWindow_->onBookmarkRequest.connect(boost::bind(&MUCController::handleBookmarkRequest, this));
	chatWindow_->onConfigureRequest.connect(boost::bind(&MUCController::handleConfigureRequest, this, _1));
	chatWindow_->onConfigurationFormCancelled.connect(boost::bind(&MUCController::handleConfigurationCancelled, this));
	chatWindow_->onDestroyRequest.connect(boost::bind(&MUCController::handleDestroyRoomRequest, this));
	chatWindow_->onInviteToChat.connect(boost::bind(&MUCController::handleInvitePersonToThisMUCRequest, this, _1));
	chatWindow_->onGetAffiliationsRequest.connect(boost::bind(&MUCController::handleGetAffiliationsRequest, this));
	chatWindow_->onChangeAffiliationsRequest.connect(boost::bind(&MUCController::handleChangeAffiliationsRequest, this, _1));
	chatWindow_->onUnblockUserRequest.connect(boost::bind(&MUCController::handleUnblockUserRequest, this));
	muc_->onJoinComplete.connect(boost::bind(&MUCController::handleJoinComplete, this, _1));
	muc_->onJoinFailed.connect(boost::bind(&MUCController::handleJoinFailed, this, _1));
	muc_->onOccupantJoined.connect(boost::bind(&MUCController::handleOccupantJoined, this, _1));
	muc_->onOccupantNicknameChanged.connect(boost::bind(&MUCController::handleOccupantNicknameChanged, this, _1, _2));
	muc_->onOccupantPresenceChange.connect(boost::bind(&MUCController::handleOccupantPresenceChange, this, _1));
	muc_->onOccupantLeft.connect(boost::bind(&MUCController::handleOccupantLeft, this, _1, _2, _3));
	muc_->onRoleChangeFailed.connect(boost::bind(&MUCController::handleOccupantRoleChangeFailed, this, _1, _2, _3));
	muc_->onAffiliationListReceived.connect(boost::bind(&MUCController::handleAffiliationListReceived, this, _1, _2));
	muc_->onConfigurationFailed.connect(boost::bind(&MUCController::handleConfigurationFailed, this, _1));
	muc_->onConfigurationFormReceived.connect(boost::bind(&MUCController::handleConfigurationFormReceived, this, _1));
	highlighter_->setMode(isImpromptu_ ? Highlighter::ChatMode : Highlighter::MUCMode);
	highlighter_->setNick(nick_);
	if (timerFactory) {
		loginCheckTimer_ = boost::shared_ptr<Timer>(timerFactory->createTimer(MUC_JOIN_WARNING_TIMEOUT_MILLISECONDS));
		loginCheckTimer_->onTick.connect(boost::bind(&MUCController::handleJoinTimeoutTick, this));
		loginCheckTimer_->start();
	}
	if (isImpromptu) {
		muc_->onUnlocked.connect(boost::bind(&MUCController::handleRoomUnlocked, this));
		chatWindow_->convertToMUC(ChatWindow::ImpromptuMUC);
	} else {
		muc_->onOccupantRoleChanged.connect(boost::bind(&MUCController::handleOccupantRoleChanged, this, _1, _2, _3));
		muc_->onOccupantAffiliationChanged.connect(boost::bind(&MUCController::handleOccupantAffiliationChanged, this, _1, _2, _3));
		chatWindow_->convertToMUC(ChatWindow::StandardMUC);
		chatWindow_->setName(muc->getJID().getNode());
	}
	setOnline(true);
	if (avatarManager_ != NULL) {
		avatarChangedConnection_ = (avatarManager_->onAvatarChanged.connect(boost::bind(&MUCController::handleAvatarChanged, this, _1)));
	} 
	handleBareJIDCapsChanged(muc->getJID());
	eventStream_->onUIEvent.connect(boost::bind(&MUCController::handleUIEvent, this, _1));


	// setup handling of MUC bookmark changes
	mucBookmarkManagerBookmarkAddedConnection_ = (mucBookmarkManager_->onBookmarkAdded.connect(boost::bind(&MUCController::handleMUCBookmarkAdded, this, _1)));
	mucBookmarkManagerBookmarkRemovedConnection_ = (mucBookmarkManager_->onBookmarkRemoved.connect(boost::bind(&MUCController::handleMUCBookmarkRemoved, this, _1)));

	std::vector<MUCBookmark> mucBookmarks = mucBookmarkManager_->getBookmarks();
	std::vector<MUCBookmark>::iterator bookmarkIterator = std::find_if(mucBookmarks.begin(), mucBookmarks.end(), MUCBookmarkPredicate(muc->getJID()));
	if (bookmarkIterator != mucBookmarks.end()) {
		updateChatWindowBookmarkStatus(*bookmarkIterator);
	}
	else {
		updateChatWindowBookmarkStatus(boost::optional<MUCBookmark>());
	}
}

MUCController::~MUCController() {
	eventStream_->onUIEvent.disconnect(boost::bind(&MUCController::handleUIEvent, this, _1));
	chatWindow_->setRosterModel(NULL);
	delete rosterVCardProvider_;
	delete roster_;
	if (loginCheckTimer_) {
		loginCheckTimer_->stop();
	}
	chatWindow_->setTabComplete(NULL);
	delete completer_;
}

void MUCController::cancelReplaces() {
	lastWasPresence_ = false;
}

void MUCController::handleWindowOccupantSelectionChanged(ContactRosterItem* item) {
	std::vector<ChatWindow::OccupantAction> actions;

	if (item) {
		MUCOccupant::Affiliation affiliation = muc_->getOccupant(getNick()).getAffiliation();
		MUCOccupant::Role role = muc_->getOccupant(getNick()).getRole();
		if (role == MUCOccupant::Moderator && !isImpromptu_)
		{
			if (affiliation == MUCOccupant::Admin || affiliation == MUCOccupant::Owner) {
				actions.push_back(ChatWindow::Ban);
			}

			actions.push_back(ChatWindow::Kick);
			actions.push_back(ChatWindow::MakeModerator);
			actions.push_back(ChatWindow::MakeParticipant);
			actions.push_back(ChatWindow::MakeVisitor);
		}
		// Add contact is available only if the real JID is also available
		if (muc_->getOccupant(item->getJID().getResource()).getRealJID()) {
			actions.push_back(ChatWindow::AddContact);
		}
		actions.push_back(ChatWindow::ShowProfile);
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
		case ChatWindow::AddContact: if (occupant.getRealJID()) events_->send(boost::make_shared<RequestAddUserDialogUIEvent>(realJID, occupant.getNick()));break;
		case ChatWindow::ShowProfile: events_->send(boost::make_shared<ShowProfileForRosterItemUIEvent>(mucJID));break;
	}
}

void MUCController::handleBareJIDCapsChanged(const JID& /*jid*/) {
	Tristate support = Yes;
	bool any = false;
	foreach (const std::string& nick, currentOccupants_) {
		DiscoInfo::ref disco = entityCapsProvider_->getCaps(toJID_.toBare().toString() + "/" + nick);
		if (disco && disco->hasFeature(DiscoInfo::MessageCorrectionFeature)) {
			any = true;
		} else {
			support = Maybe;
		}
	}
	if (!any) {
		support = No;
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
#ifdef SWIFT_EXPERIMENTAL_HISTORY
		if (lastActivity_ == boost::posix_time::not_a_date_time && historyController_) {
			lastActivity_ = historyController_->getLastTimeStampFromMUC(selfJID_, toJID_);
		}
#endif
		if (lastActivity_ == boost::posix_time::not_a_date_time) {
			muc_->joinAs(nick_);
		}
		else {
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

const boost::optional<std::string> MUCController::getPassword() const {
	return password_;
}

bool MUCController::isImpromptu() const {
	return isImpromptu_;
}

std::map<std::string, JID> MUCController::getParticipantJIDs() const {
	std::map<std::string, JID> participants;
	typedef std::pair<std::string, MUCOccupant> MUCOccupantPair;
	std::map<std::string, MUCOccupant> occupants = muc_->getOccupants();
	foreach(const MUCOccupantPair& occupant, occupants) {
		if (occupant.first != nick_) {
			participants[occupant.first] = occupant.second.getRealJID().is_initialized() ? occupant.second.getRealJID().get().toBare() : JID();
		}
	}
	return participants;
}

void MUCController::sendInvites(const std::vector<JID>& jids, const std::string& reason) const {
	foreach (const JID& jid, jids) {
		muc_->invitePerson(jid, reason, isImpromptu_);
	}
}

void MUCController::handleJoinTimeoutTick() {
	receivedActivity();
	chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(str(format(QT_TRANSLATE_NOOP("", "Room %1% is not responding. This operation may never complete.")) % toJID_.toString())), ChatWindow::DefaultDirection);
}

void MUCController::receivedActivity() {
	if (loginCheckTimer_) {
		loginCheckTimer_->stop();
	}
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"

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
	errorMessage = str(format(QT_TRANSLATE_NOOP("", "Couldn't enter room: %1%.")) % errorMessage);
	chatWindow_->addErrorMessage(chatMessageParser_->parseMessageBody(errorMessage));
	parting_ = true;
	if (!rejoinNick.empty() && renameCounter_ < 10) {
		renameCounter_++;
		setNick(rejoinNick);
		rejoin();
	}
}

#pragma clang diagnostic pop

void MUCController::handleJoinComplete(const std::string& nick) {
	receivedActivity();
	renameCounter_ = 0;
	joined_ = true;
	std::string joinMessage;
	if (isImpromptu_) {
		joinMessage = str(format(QT_TRANSLATE_NOOP("", "You have joined the chat as %1%.")) % nick);
	} else {
		joinMessage = str(format(QT_TRANSLATE_NOOP("", "You have entered room %1% as %2%.")) % toJID_.toString() % nick);
	}
	setNick(nick);
	chatWindow_->replaceSystemMessage(chatMessageParser_->parseMessageBody(joinMessage), lastJoinMessageUID_, ChatWindow::UpdateTimestamp);
	lastJoinMessageUID_ = "";

#ifdef SWIFT_EXPERIMENTAL_HISTORY
	addRecentLogs();
#endif

	clearPresenceQueue();
	shouldJoinOnReconnect_ = true;
	setEnabled(true);
	if (isImpromptu_) {
		setAvailableRoomActions(MUCOccupant::NoAffiliation, MUCOccupant::Participant);
	} else {
		MUCOccupant occupant = muc_->getOccupant(nick);
		setAvailableRoomActions(occupant.getAffiliation(), occupant.getRole());
	}
	onUserJoined();

	if (isImpromptu_) {
		setImpromptuWindowTitle();
	}
}

void MUCController::handleAvatarChanged(const JID& jid) {
	if (parting_ || !jid.equals(toJID_, JID::WithoutResource)) {
		return;
	}
	roster_->applyOnItems(SetAvatar(jid, avatarManager_->getAvatarPath(jid), JID::WithResource));
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
	MUCOccupant::Role role = MUCOccupant::Participant;
	MUCOccupant::Affiliation affiliation = MUCOccupant::NoAffiliation;
	if (!isImpromptu_) {
		role = occupant.getRole();
		affiliation = occupant.getAffiliation();
	}
	std::string groupName(roleToGroupName(role));
	roster_->addContact(jid, realJID, occupant.getNick(), groupName, avatarManager_->getAvatarPath(jid));
	roster_->applyOnItems(SetMUC(jid, role, affiliation));
	roster_->getGroup(groupName)->setManualSort(roleToSortName(role));
	if (joined_) {
		std::string joinString;
		if (role != MUCOccupant::NoRole && role != MUCOccupant::Participant) {
			joinString = str(format(QT_TRANSLATE_NOOP("", "%1% has entered the %3% as a %2%.")) % occupant.getNick() % roleToFriendlyName(role) % (isImpromptu_ ? QT_TRANSLATE_NOOP("", "chat") : QT_TRANSLATE_NOOP("", "room")));
		}
		else {
			joinString = str(format(QT_TRANSLATE_NOOP("", "%1% has entered the %2%.")) % occupant.getNick() % (isImpromptu_ ? QT_TRANSLATE_NOOP("", "chat") : QT_TRANSLATE_NOOP("", "room")));
		}
		if (shouldUpdateJoinParts()) {
			updateJoinParts();
		} else {
			addPresenceMessage(joinString);
		}

		if (isImpromptu_) {
			setImpromptuWindowTitle();
			onActivity("");
		}
	}
	if (avatarManager_ != NULL) {
		handleAvatarChanged(jid);
	}
}

void MUCController::addPresenceMessage(const std::string& message) {
	lastWasPresence_ = true;
	chatWindow_->addPresenceMessage(chatMessageParser_->parseMessageBody(message), ChatWindow::DefaultDirection);
}


void MUCController::setAvailableRoomActions(const MUCOccupant::Affiliation& affiliation, const MUCOccupant::Role& role)
{
	std::vector<ChatWindow::RoomAction> actions;

	if (role <= MUCOccupant::Participant) {
		actions.push_back(ChatWindow::ChangeSubject);
	}
	if (affiliation == MUCOccupant::Owner) {
		actions.push_back(ChatWindow::Configure);
	}
	if (affiliation <= MUCOccupant::Admin) {
		actions.push_back(ChatWindow::Affiliations);
	}
	if (affiliation == MUCOccupant::Owner) {
		actions.push_back(ChatWindow::Destroy);
	}
	if (role <= MUCOccupant::Visitor) {
		actions.push_back(ChatWindow::Invite);
	}
	chatWindow_->setAvailableRoomActions(actions);
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
	assert(false);
	return "";
}

std::string MUCController::roleToSortName(MUCOccupant::Role role) {
	switch (role) {
	case MUCOccupant::Moderator: return "1";
	case MUCOccupant::Participant: return "2";
	case MUCOccupant::Visitor: return "3";
	case MUCOccupant::NoRole: return "4";
	}
	assert(false);
	return "5";
}

JID MUCController::nickToJID(const std::string& nick) {
	return muc_->getJID().withResource(nick);
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
	if (joined_ && messageEvent->getStanza()->getFrom().getResource() != nick_ && messageTargetsMe(message) && !message->getPayload<Delay>() && messageEvent->isReadable()) {
		chatWindow_->flash();
	}
	else {
		messageEvent->setTargetsMe(false);
	}
	if (messageEvent->isReadable() && isImpromptu_) {
		chatWindow_->flash(); /* behave like a regular char*/
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
		chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(str(format(QT_TRANSLATE_NOOP("", "The room subject is now: %1%")) % message->getSubject())), ChatWindow::DefaultDirection);
		chatWindow_->setSubject(message->getSubject());
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_ && !message->getPayload<Delay>()) {
		doneGettingHistory_ = true;
	}

	if (!doneGettingHistory_) {
		checkDuplicates(message);
		messageEvent->conclude();
	}
}

void MUCController::addMessageHandleIncomingMessage(const JID& from, const std::string& message, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	if (from.isBare()) {
		chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(str(format(QT_TRANSLATE_NOOP("", "%1%")) % message)), ChatWindow::DefaultDirection);
	}
	else {
		ChatControllerBase::addMessageHandleIncomingMessage(from, message, senderIsSelf, label, time, highlight);
	}
}

void MUCController::postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent, const HighlightAction& highlight) {
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	if (joined_ && messageEvent->getStanza()->getFrom().getResource() != nick_ && !message->getPayload<Delay>()) {
		if (messageTargetsMe(message) || isImpromptu_) {
			eventController_->handleIncomingEvent(messageEvent);
			if (!messageEvent->getConcluded()) {
				highlighter_->handleHighlightAction(highlight);
			}
		}
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
	roster_->addContact(jid, realJID, nick, group, avatarManager_->getAvatarPath(jid));
	roster_->getGroup(group)->setManualSort(roleToSortName(occupant.getRole()));
	roster_->applyOnItems(SetMUC(jid, occupant.getRole(), occupant.getAffiliation()));
	chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(str(format(QT_TRANSLATE_NOOP("", "%1% is now a %2%")) % nick % roleToFriendlyName(occupant.getRole()))), ChatWindow::DefaultDirection);
	if (nick == nick_) {
		setAvailableRoomActions(occupant.getAffiliation(), occupant.getRole());
	}
}

void MUCController::handleOccupantAffiliationChanged(const std::string& nick, const MUCOccupant::Affiliation& affiliation, const MUCOccupant::Affiliation& /*oldAffiliation*/)
{
	if (nick == nick_) {
		setAvailableRoomActions(affiliation, muc_->getOccupant(nick_).getRole());
	}
	JID jid(nickToJID(nick));
	MUCOccupant occupant = muc_->getOccupant(nick);
	roster_->applyOnItems(SetMUC(jid, occupant.getRole(), affiliation));
}

std::string MUCController::roleToGroupName(MUCOccupant::Role role) {
	std::string result;
	switch (role) {
	case MUCOccupant::Moderator: result = QT_TRANSLATE_NOOP("", "Moderators"); break;
	case MUCOccupant::Participant: result = QT_TRANSLATE_NOOP("", "Participants"); break;
	case MUCOccupant::Visitor: result = QT_TRANSLATE_NOOP("", "Visitors"); break;
	case MUCOccupant::NoRole: result = QT_TRANSLATE_NOOP("", "Occupants"); break;
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
			renameCounter_ = 0;
			boost::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();
			if (blockList && blockList->isBlocked(muc_->getJID())) {
				handleBlockingStateChanged();
				lastJoinMessageUID_ = chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(QT_TRANSLATE_NOOP("", "You've blocked this room. To enter the room, first unblock it using the cog menu and try again")), ChatWindow::DefaultDirection);
			}
			else {
				if (isImpromptu_) {
					lastJoinMessageUID_ = chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(QT_TRANSLATE_NOOP("", "Trying to join chat")), ChatWindow::DefaultDirection);
				} else {
					lastJoinMessageUID_ = chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(str(format(QT_TRANSLATE_NOOP("", "Trying to enter room %1%")) % toJID_.toString())), ChatWindow::DefaultDirection);
				}
				if (loginCheckTimer_) {
					loginCheckTimer_->start();
				}
				setNick(desiredNick_);
				rejoin();
			}
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
		if (isImpromptu_) {
			partMessage = str(format(QT_TRANSLATE_NOOP("", "%1% has left the chat%2%")) % occupant.getNick() % partType);
		} else {
			partMessage = str(format(QT_TRANSLATE_NOOP("", "%1% has left the room%2%")) % occupant.getNick() % partType);
		}
	}
	else if (isImpromptu_) {
		switch (type) {
			case MUC::LeaveKick:
			case MUC::LeaveBan: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "You have been removed from this chat"); break;
			case MUC::LeaveNotMember: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "You have been removed from this chat"); break;
			case MUC::LeaveDestroy: clearPresenceQueue(); clearAfter = true; partMessage = QT_TRANSLATE_NOOP("", "This chat has ended"); break;
			case MUC::Disconnect:
			case MUC::LeavePart: partMessage = QT_TRANSLATE_NOOP("", "You have left the chat");
		}
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

	if (isImpromptu_) {
		setImpromptuWindowTitle();
	}
}

void MUCController::handleOccupantNicknameChanged(const std::string& oldNickname, const std::string& newNickname) {
	addPresenceMessage(generateNicknameChangeString(oldNickname, newNickname));
	JID oldJID = muc_->getJID().withResource(oldNickname);
	JID newJID = muc_->getJID().withResource(newNickname);

	// adjust occupants
	currentOccupants_.erase(oldNickname);
	currentOccupants_.insert(newNickname);

	// adjust completer
	completer_->removeWord(oldNickname);
	completer_->addWord(newNickname);

	// update contact
	roster_->removeContact(oldJID);
	MUCOccupant occupant = muc_->getOccupant(newNickname);

	JID realJID;
	if (occupant.getRealJID()) {
		realJID = occupant.getRealJID().get();
	}
	MUCOccupant::Role role = MUCOccupant::Participant;
	MUCOccupant::Affiliation affiliation = MUCOccupant::NoAffiliation;
	if (!isImpromptu_) {
		role = occupant.getRole();
		affiliation = occupant.getAffiliation();
	}
	std::string groupName(roleToGroupName(role));
	roster_->addContact(newJID, realJID, newNickname, groupName, avatarManager_->getAvatarPath(newJID));
	roster_->applyOnItems(SetMUC(newJID, role, affiliation));
	if (avatarManager_ != NULL) {
		handleAvatarChanged(newJID);
	}

	clearPresenceQueue();
	onUserNicknameChanged(oldNickname, newNickname);
}

void MUCController::handleOccupantPresenceChange(boost::shared_ptr<Presence> presence) {
	receivedActivity();
	roster_->applyOnItems(SetPresence(presence, JID::WithResource));
}

bool MUCController::isIncomingMessageFromMe(boost::shared_ptr<Message> message) {
	JID from = message->getFrom();
	return nick_ == from.getResource();
}

std::string MUCController::senderHighlightNameFromMessage(const JID& from) {
	return from.getResource();
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
	chatWindow_->replaceLastMessage(chatMessageParser_->parseMessageBody(generateJoinPartString(joinParts_, isImpromptu())), ChatWindow::UpdateTimestamp);
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
				case PartThenJoin: break;
				case JoinThenPart: break;
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

std::string MUCController::generateJoinPartString(const std::vector<NickJoinPart>& joinParts, bool isImpromptu) {
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
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% have joined the chat") : QT_TRANSLATE_NOOP("", "%1% have entered the room"));
					}
					else {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% has joined the chat") : QT_TRANSLATE_NOOP("", "%1% has entered the room"));
					}
					break;
				case Part: 
					if (sorted[i].size() > 1) {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% have left the chat") : QT_TRANSLATE_NOOP("", "%1% have left the room"));
					}
					else {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% have left the chat") : QT_TRANSLATE_NOOP("", "%1% has left the room"));
					}
					break;
				case JoinThenPart: 
					if (sorted[i].size() > 1) {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% have joined then left the chat") : QT_TRANSLATE_NOOP("", "%1% have entered then left the room"));
					}
					else {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% has joined then left the chat") : QT_TRANSLATE_NOOP("", "%1% has entered then left the room"));
					}
					break;
				case PartThenJoin: 
					if (sorted[i].size() > 1) {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% have left then returned to the chat") : QT_TRANSLATE_NOOP("", "%1% have left then returned to the room"));
					}
					else {
						eventString = (isImpromptu ? QT_TRANSLATE_NOOP("", "%1% has left then returned to the chat") : QT_TRANSLATE_NOOP("", "%1% has left then returned to the room"));
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

std::string MUCController::generateNicknameChangeString(const std::string& oldNickname, const std::string& newNickname) {
	return str(boost::format(QT_TRANSLATE_NOOP("", "%1% is now known as %2%.")) % oldNickname % newNickname);
}

void MUCController::handleChangeSubjectRequest(const std::string& subject) {
	muc_->changeSubject(subject);
}

void MUCController::handleBookmarkRequest() {
	const JID jid = muc_->getJID();
	MUCBookmark bookmark(jid, jid.toBare().toString());
	bookmark.setPassword(password_);
	bookmark.setNick(nick_);
	chatWindow_->showBookmarkWindow(bookmark);
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
	chatWindow_->addErrorMessage(chatMessageParser_->parseMessageBody(errorMessage));
}

void MUCController::handleOccupantRoleChangeFailed(ErrorPayload::ref error, const JID&, MUCOccupant::Role) {
	std::string errorMessage = getErrorMessage(error);
	errorMessage = str(format(QT_TRANSLATE_NOOP("", "Occupant role change failed: %1%.")) % errorMessage);
	chatWindow_->addErrorMessage(chatMessageParser_->parseMessageBody(errorMessage));
}

void MUCController::configureAsImpromptuRoom(Form::ref form) {
	muc_->configureRoom(buildImpromptuRoomConfiguration(form));
	isImpromptuAlreadyConfigured_ = true;
	onImpromptuConfigCompleted();
}

void MUCController::handleConfigurationFormReceived(Form::ref form) {
	if (isImpromptu_) {
		if (!isImpromptuAlreadyConfigured_) {
			configureAsImpromptuRoom(form);
		}
	} else {
		chatWindow_->showRoomConfigurationForm(form);
	}
}

void MUCController::handleConfigurationCancelled() {
	muc_->cancelConfigureRoom();
}

void MUCController::handleDestroyRoomRequest() {
	muc_->destroyRoom();
}

void MUCController::handleInvitePersonToThisMUCRequest(const std::vector<JID>& jidsToInvite) {
	RequestInviteToMUCUIEvent::ImpromptuMode mode = isImpromptu_ ? RequestInviteToMUCUIEvent::Impromptu : RequestInviteToMUCUIEvent::NotImpromptu;
	boost::shared_ptr<UIEvent> event(new RequestInviteToMUCUIEvent(muc_->getJID(), jidsToInvite, mode));
	eventStream_->send(event);
}

void MUCController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<InviteToMUCUIEvent> inviteEvent = boost::dynamic_pointer_cast<InviteToMUCUIEvent>(event);
	if (inviteEvent && inviteEvent->getRoom() == muc_->getJID()) {
		foreach (const JID& jid, inviteEvent->getInvites()) {
			muc_->invitePerson(jid, inviteEvent->getReason(), isImpromptu_);
		}
	}
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

void MUCController::handleUnblockUserRequest() {
	eventStream_->send(boost::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, muc_->getJID()));
}

void MUCController::handleBlockingStateChanged() {
	boost::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();
	if (blockList->getState() == BlockList::Available) {
		if (blockList->isBlocked(toJID_)) {
			if (!blockedContactAlert_) {
				blockedContactAlert_ = chatWindow_->addAlert(QT_TRANSLATE_NOOP("", "You've blocked this room. To enter the room, first unblock it using the cog menu and try again"));
			}
			chatWindow_->setBlockingState(ChatWindow::IsBlocked);
		} else {
			if (blockedContactAlert_) {
				chatWindow_->removeAlert(*blockedContactAlert_);
				blockedContactAlert_.reset();
			}
			chatWindow_->setBlockingState(ChatWindow::IsUnblocked);
		}
	}
}

void MUCController::handleAffiliationListReceived(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
	chatWindow_->setAffiliations(affiliation, jids);
}

void MUCController::logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming) {
	// log only incoming messages
	if (isIncoming && historyController_) {
		historyController_->addMessage(message, fromJID, toJID, HistoryMessage::Groupchat, timeStamp);
	}
}

void MUCController::addRecentLogs() {
	if (!historyController_) {
		return;
	}

	joinContext_ = historyController_->getMUCContext(selfJID_, toJID_, lastActivity_);

	foreach (const HistoryMessage& message, joinContext_) {
		bool senderIsSelf = nick_ == message.getFromJID().getResource();

		// the chatWindow uses utc timestamps
		addMessage(message.getMessage(), senderDisplayNameFromMessage(message.getFromJID()), senderIsSelf, boost::shared_ptr<SecurityLabel>(new SecurityLabel()), avatarManager_->getAvatarPath(message.getFromJID()), message.getTime() - boost::posix_time::hours(message.getOffset()), HighlightAction());
	}
}

void MUCController::checkDuplicates(boost::shared_ptr<Message> newMessage) {
	std::string body = newMessage->getBody();
	JID jid = newMessage->getFrom();
	boost::optional<boost::posix_time::ptime> time = newMessage->getTimestamp();

	reverse_foreach (const HistoryMessage& message, joinContext_) {
		boost::posix_time::ptime messageTime = message.getTime() - boost::posix_time::hours(message.getOffset());
		if (time && time < messageTime) {
			break;
		}
		if (time && time != messageTime) {
			continue;
		}
		if (message.getFromJID() != jid) {
			continue;
		}
		if (message.getMessage() != body) {
			continue;
		}

		// Mark the message as unreadable
		newMessage->setBody("");
	}
}

void MUCController::setNick(const std::string& nick) {
	nick_ = nick;
	highlighter_->setNick(nick_);
}

Form::ref MUCController::buildImpromptuRoomConfiguration(Form::ref roomConfigurationForm) {
	Form::ref result = boost::make_shared<Form>(Form::SubmitType);
	std::string impromptuConfigs[] = { "muc#roomconfig_enablelogging", "muc#roomconfig_persistentroom", "muc#roomconfig_publicroom", "muc#roomconfig_whois"};
	std::set<std::string> impromptuConfigsMissing(impromptuConfigs, impromptuConfigs + 4);
	foreach (boost::shared_ptr<FormField> field, roomConfigurationForm->getFields()) {
		boost::shared_ptr<FormField> resultField;
		if (field->getName() == "muc#roomconfig_enablelogging") {
			resultField = boost::make_shared<FormField>(FormField::BooleanType, "0");
		}
		if (field->getName() == "muc#roomconfig_persistentroom") {
			resultField = boost::make_shared<FormField>(FormField::BooleanType, "0");
		}
		if (field->getName() == "muc#roomconfig_publicroom") {
			resultField = boost::make_shared<FormField>(FormField::BooleanType, "0");
		}
		if (field->getName() == "muc#roomconfig_whois") {
			resultField = boost::make_shared<FormField>(FormField::ListSingleType, "anyone");
		}

		if (field->getName() == "FORM_TYPE") {
			resultField = boost::make_shared<FormField>(FormField::HiddenType, "http://jabber.org/protocol/muc#roomconfig");
		}

		if (resultField) {
			impromptuConfigsMissing.erase(field->getName());
			resultField->setName(field->getName());
			result->addField(resultField);
		}
	}

	foreach (const std::string& config, impromptuConfigsMissing) {
		if (config == "muc#roomconfig_publicroom") {
			chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(QT_TRANSLATE_NOOP("", "This server doesn't support hiding your chat from other users.")), ChatWindow::DefaultDirection);
		} else if (config == "muc#roomconfig_whois") {
			chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(QT_TRANSLATE_NOOP("", "This server doesn't support sharing people's real identity in this chat.")), ChatWindow::DefaultDirection);
		}
	}

	return result;
}

void MUCController::setImpromptuWindowTitle() {
	std::string title;
	typedef std::pair<std::string, MUCOccupant> StringMUCOccupantPair;
	std::map<std::string, MUCOccupant> occupants = muc_->getOccupants();
	if (occupants.size() <= 1) {
		title = QT_TRANSLATE_NOOP("", "Empty Chat");
	} else {
		foreach (StringMUCOccupantPair pair, occupants) {
			if (pair.first != nick_) {
				title += (title.empty() ? "" : ", ") + pair.first;
			}
		}
	}
	chatWindow_->setName(title);
}

void MUCController::handleRoomUnlocked() {
	// Handle buggy MUC implementations where the joined room already exists and is unlocked.
	// Configure the room again in this case.
	if (!isImpromptuAlreadyConfigured_) {
		if (isImpromptu_ && (muc_->getOccupant(nick_).getAffiliation() == MUCOccupant::Owner)) {
			muc_->requestConfigurationForm();
		} else if (isImpromptu_) {
			onImpromptuConfigCompleted();
		}
	}
}

void MUCController::setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info) {
	ChatControllerBase::setAvailableServerFeatures(info);
	if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::BlockingCommandFeature)) {
		boost::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();

		blockingOnStateChangedConnection_ = blockList->onStateChanged.connect(boost::bind(&MUCController::handleBlockingStateChanged, this));
		blockingOnItemAddedConnection_ = blockList->onItemAdded.connect(boost::bind(&MUCController::handleBlockingStateChanged, this));
		blockingOnItemRemovedConnection_ = blockList->onItemRemoved.connect(boost::bind(&MUCController::handleBlockingStateChanged, this));

		handleBlockingStateChanged();
	}
}

void MUCController::handleMUCBookmarkAdded(const MUCBookmark& bookmark) {
	if (bookmark.getRoom() == muc_->getJID()) {
		updateChatWindowBookmarkStatus(bookmark);
	}
}

void MUCController::handleMUCBookmarkRemoved(const MUCBookmark& bookmark) {
	if (bookmark.getRoom() == muc_->getJID()) {
		updateChatWindowBookmarkStatus(boost::optional<MUCBookmark>());
	}
}

void MUCController::updateChatWindowBookmarkStatus(const boost::optional<MUCBookmark>& bookmark) {
	assert(chatWindow_);
	if (bookmark) {
		if (bookmark->getAutojoin()) {
			chatWindow_->setBookmarkState(ChatWindow::RoomAutoJoined);
		}
		else {
			chatWindow_->setBookmarkState(ChatWindow::RoomBookmarked);
		}
	}
	else {
		chatWindow_->setBookmarkState(ChatWindow::RoomNotBookmarked);
	}
}

}
