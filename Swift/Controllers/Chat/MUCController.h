/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <set>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/signals/connection.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUC.h>
#include <Swiften/Network/Timer.h>

#include <Swift/Controllers/Chat/ChatControllerBase.h>
#include <Swift/Controllers/Roster/RosterItem.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
	class StanzaChannel;
	class IQRouter;
	class ChatWindowFactory;
	class Roster;
	class AvatarManager;
	class UIEventStream;
	class TimerFactory;
	class TabComplete;
	class XMPPRoster;
	class HighlightManager;
	class UIEvent;
	class VCardManager;
	class RosterVCardProvider;
	class ClientBlockListManager;
	class MUCBookmarkManager;
	class MUCBookmark;

	enum JoinPart {Join, Part, JoinThenPart, PartThenJoin};

	struct NickJoinPart {
			NickJoinPart(const std::string& nick, JoinPart type) : nick(nick), type(type) {}
			std::string nick;
			JoinPart type;
	};

	class MUCController : public ChatControllerBase {
		public:
			MUCController(const JID& self, MUC::ref muc, const boost::optional<std::string>& password, const std::string &nick, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, PresenceOracle* presenceOracle, AvatarManager* avatarManager, UIEventStream* events, bool useDelayForLatency, TimerFactory* timerFactory, EventController* eventController, EntityCapsProvider* entityCapsProvider, XMPPRoster* roster, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, boost::shared_ptr<ChatMessageParser> chatMessageParser, bool isImpromptu, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider, VCardManager* vcardManager, MUCBookmarkManager* mucBookmarkManager);
			virtual ~MUCController();
			boost::signal<void ()> onUserLeft;
			boost::signal<void ()> onUserJoined;
			boost::signal<void ()> onImpromptuConfigCompleted;
			boost::signal<void (const std::string&, const std::string& )> onUserNicknameChanged;
			virtual void setOnline(bool online);
			virtual void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			void rejoin();
			static void appendToJoinParts(std::vector<NickJoinPart>& joinParts, const NickJoinPart& newEvent);
			static std::string generateJoinPartString(const std::vector<NickJoinPart>& joinParts, bool isImpromptu);
			static std::string concatenateListOfNames(const std::vector<NickJoinPart>& joinParts);
			static std::string generateNicknameChangeString(const std::string& oldNickname, const std::string& newNickname);
			bool isJoined();
			const std::string& getNick();
			const boost::optional<std::string> getPassword() const;
			bool isImpromptu() const;
			std::map<std::string, JID> getParticipantJIDs() const;
			void sendInvites(const std::vector<JID>& jids, const std::string& reason) const;
		
		protected:
			void preSendMessageRequest(boost::shared_ptr<Message> message);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			std::string senderHighlightNameFromMessage(const JID& from);
			std::string senderDisplayNameFromMessage(const JID& from);
			boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message> message) const;
			virtual void preHandleIncomingMessage(boost::shared_ptr<MessageEvent>);
			virtual void addMessageHandleIncomingMessage(const JID& from, const std::string& message, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			virtual void postHandleIncomingMessage(boost::shared_ptr<MessageEvent>, const HighlightAction&);
			void cancelReplaces();
			void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming);

		private:
			void setAvailableRoomActions(const MUCOccupant::Affiliation& affiliation, const MUCOccupant::Role& role);
			void clearPresenceQueue();
			void addPresenceMessage(const std::string& message);
			void handleWindowOccupantSelectionChanged(ContactRosterItem* item);
			void handleActionRequestedOnOccupant(ChatWindow::OccupantAction, ContactRosterItem* item);
			void handleWindowClosed();
			void handleAvatarChanged(const JID& jid);
			void handleOccupantJoined(const MUCOccupant& occupant);
			void handleOccupantNicknameChanged(const std::string& oldNickname, const std::string& newNickname);
			void handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const std::string& reason);
			void handleOccupantPresenceChange(boost::shared_ptr<Presence> presence);
			void handleOccupantRoleChanged(const std::string& nick, const MUCOccupant& occupant,const MUCOccupant::Role& oldRole);
			void handleOccupantAffiliationChanged(const std::string& nick, const MUCOccupant::Affiliation& affiliation,const MUCOccupant::Affiliation& oldAffiliation);
			void handleJoinComplete(const std::string& nick);
			void handleJoinFailed(boost::shared_ptr<ErrorPayload> error);
			void handleJoinTimeoutTick();
			void handleChangeSubjectRequest(const std::string&);
			void handleBookmarkRequest();
			std::string roleToGroupName(MUCOccupant::Role role);
			std::string roleToSortName(MUCOccupant::Role role);
			JID nickToJID(const std::string& nick);
			std::string roleToFriendlyName(MUCOccupant::Role role);
			void receivedActivity();
			bool messageTargetsMe(boost::shared_ptr<Message> message);
			void updateJoinParts();
			bool shouldUpdateJoinParts();
			void dayTicked() {clearPresenceQueue();}
			void processUserPart();
			void handleBareJIDCapsChanged(const JID& jid);
			void handleConfigureRequest(Form::ref);
			void handleConfigurationFailed(ErrorPayload::ref);
			void handleConfigurationFormReceived(Form::ref);
			void handleDestroyRoomRequest();
			void handleInvitePersonToThisMUCRequest(const std::vector<JID>& jidsToInvite);
			void handleConfigurationCancelled();
			void handleOccupantRoleChangeFailed(ErrorPayload::ref, const JID&, MUCOccupant::Role);
			void handleGetAffiliationsRequest();
			void handleAffiliationListReceived(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids);
			void handleChangeAffiliationsRequest(const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& changes);
			void handleInviteToMUCWindowDismissed();
			void handleInviteToMUCWindowCompleted();
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void addRecentLogs();
			void checkDuplicates(boost::shared_ptr<Message> newMessage);
			void setNick(const std::string& nick);
			void setImpromptuWindowTitle();
			void handleRoomUnlocked();
			void configureAsImpromptuRoom(Form::ref form);
			Form::ref buildImpromptuRoomConfiguration(Form::ref roomConfigurationForm);

			void handleUnblockUserRequest();
			void handleBlockingStateChanged();

			void handleMUCBookmarkAdded(const MUCBookmark& bookmark);
			void handleMUCBookmarkRemoved(const MUCBookmark& bookmark);
			void updateChatWindowBookmarkStatus(const boost::optional<MUCBookmark>& bookmark);

		private:
			MUC::ref muc_;
			UIEventStream* events_;
			std::string nick_;
			std::string desiredNick_;
			Roster* roster_;
			TabComplete* completer_;
			bool parting_;
			bool joined_;
			bool lastWasPresence_;
			bool shouldJoinOnReconnect_;
			bool doneGettingHistory_;
			boost::bsignals::scoped_connection avatarChangedConnection_;
			boost::shared_ptr<Timer> loginCheckTimer_;
			std::set<std::string> currentOccupants_;
			std::vector<NickJoinPart> joinParts_;
			boost::posix_time::ptime lastActivity_;
			boost::optional<std::string> password_;
			XMPPRoster* xmppRoster_;
			std::vector<HistoryMessage> joinContext_;
			size_t renameCounter_;
			bool isImpromptu_;
			bool isImpromptuAlreadyConfigured_;
			RosterVCardProvider* rosterVCardProvider_;
			std::string lastJoinMessageUID_;

			ClientBlockListManager* clientBlockListManager_;
			boost::bsignals::scoped_connection blockingOnStateChangedConnection_;
			boost::bsignals::scoped_connection blockingOnItemAddedConnection_;
			boost::bsignals::scoped_connection blockingOnItemRemovedConnection_;

			boost::optional<ChatWindow::AlertID> blockedContactAlert_;

			MUCBookmarkManager* mucBookmarkManager_;
			boost::bsignals::scoped_connection mucBookmarkManagerBookmarkAddedConnection_;
			boost::bsignals::scoped_connection mucBookmarkManagerBookmarkRemovedConnection_;
	};
}

