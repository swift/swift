/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include <boost/signals2.hpp>
#include <boost/signals2/connection.hpp>

#include <Swiften/Base/Override.h>
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
            MUCController(const JID& self, MUC::ref muc, const boost::optional<std::string>& password, const std::string &nick, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, UIEventStream* events, bool useDelayForLatency, TimerFactory* timerFactory, EventController* eventController, EntityCapsProvider* entityCapsProvider, XMPPRoster* xmppRoster, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, std::shared_ptr<ChatMessageParser> chatMessageParser, bool isImpromptu, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider, VCardManager* vcardManager, MUCBookmarkManager* mucBookmarkManager);
            virtual ~MUCController();
            boost::signals2::signal<void ()> onUserLeft;
            boost::signals2::signal<void ()> onUserJoined;
            boost::signals2::signal<void ()> onImpromptuConfigCompleted;
            boost::signals2::signal<void (const std::string&, const std::string& )> onUserNicknameChanged;
            virtual void setOnline(bool online) SWIFTEN_OVERRIDE;
            virtual void setAvailableServerFeatures(std::shared_ptr<DiscoInfo> info) SWIFTEN_OVERRIDE;
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
            void setChatWindowTitle(const std::string& title);

        protected:
            virtual void preSendMessageRequest(std::shared_ptr<Message> message) SWIFTEN_OVERRIDE;
            virtual bool isIncomingMessageFromMe(std::shared_ptr<Message> message) SWIFTEN_OVERRIDE;
            virtual std::string senderHighlightNameFromMessage(const JID& from) SWIFTEN_OVERRIDE;
            virtual std::string senderDisplayNameFromMessage(const JID& from) SWIFTEN_OVERRIDE;
            virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(std::shared_ptr<Message> message) const SWIFTEN_OVERRIDE;
            virtual void preHandleIncomingMessage(std::shared_ptr<MessageEvent>) SWIFTEN_OVERRIDE;
            virtual void addMessageHandleIncomingMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& time) SWIFTEN_OVERRIDE;
            virtual void handleIncomingReplaceMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, const std::string& idToReplace, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& timeStamp) SWIFTEN_OVERRIDE;
            virtual void postHandleIncomingMessage(std::shared_ptr<MessageEvent>, const ChatWindow::ChatMessage& chatMessage) SWIFTEN_OVERRIDE;
            virtual void cancelReplaces() SWIFTEN_OVERRIDE;
            virtual void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming) SWIFTEN_OVERRIDE;
            virtual JID messageCorrectionJID(const JID& fromJID) SWIFTEN_OVERRIDE;

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
            void handleOccupantPresenceChange(std::shared_ptr<Presence> presence);
            void handleOccupantRoleChanged(const std::string& nick, const MUCOccupant& occupant,const MUCOccupant::Role& oldRole);
            void handleOccupantAffiliationChanged(const std::string& nick, const MUCOccupant::Affiliation& affiliation,const MUCOccupant::Affiliation& oldAffiliation);
            void handleJoinComplete(const std::string& nick);
            void handleJoinFailed(std::shared_ptr<ErrorPayload> error);
            void handleJoinTimeoutTick();
            void handleChangeSubjectRequest(const std::string&);
            void handleBookmarkRequest();
            std::string roleToGroupName(MUCOccupant::Role role);
            std::string roleToSortName(MUCOccupant::Role role);
            JID nickToJID(const std::string& nick);
            std::string roleToFriendlyName(MUCOccupant::Role role);
            void receivedActivity();
            bool messageTargetsMe(std::shared_ptr<Message> message);
            void updateJoinParts();
            bool shouldUpdateJoinParts();
            virtual void dayTicked() SWIFTEN_OVERRIDE { clearPresenceQueue(); }
            void processUserPart();
            virtual void handleBareJIDCapsChanged(const JID& jid) SWIFTEN_OVERRIDE;
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
            void handleUIEvent(std::shared_ptr<UIEvent> event);
            void addRecentLogs();
            void checkDuplicates(std::shared_ptr<Message> newMessage);
            void setNick(const std::string& nick);
            void handleRoomUnlocked();
            void configureAsImpromptuRoom(Form::ref form);
            Form::ref buildImpromptuRoomConfiguration(Form::ref roomConfigurationForm);

            void handleUnblockUserRequest();
            void handleBlockingStateChanged();

            void handleMUCBookmarkAdded(const MUCBookmark& bookmark);
            void handleMUCBookmarkRemoved(const MUCBookmark& bookmark);
            void updateChatWindowBookmarkStatus(const boost::optional<MUCBookmark>& bookmark);

            void displaySubjectIfChanged(const std::string& sucject);
            void addChatSystemMessage();

        private:
            MUC::ref muc_;
            std::string nick_;
            std::string desiredNick_;
            TabComplete* completer_;
            bool parting_;
            bool joined_;
            bool shouldJoinOnReconnect_;
            bool doneGettingHistory_;
            boost::signals2::scoped_connection avatarChangedConnection_;
            std::shared_ptr<Timer> loginCheckTimer_;
            std::set<std::string> currentOccupants_;
            std::vector<NickJoinPart> joinParts_;
            boost::posix_time::ptime lastActivity_;
            boost::optional<std::string> password_;
            XMPPRoster* xmppRoster_;
            std::unique_ptr<Roster> roster_;
            std::vector<HistoryMessage> joinContext_;
            size_t renameCounter_;
            bool isImpromptu_;
            bool isImpromptuAlreadyConfigured_;
            RosterVCardProvider* rosterVCardProvider_;
            std::string lastJoinMessageUID_;
            std::string lastStartMessage_;

            ClientBlockListManager* clientBlockListManager_;
            boost::signals2::scoped_connection blockingOnStateChangedConnection_;
            boost::signals2::scoped_connection blockingOnItemAddedConnection_;
            boost::signals2::scoped_connection blockingOnItemRemovedConnection_;

            boost::optional<ChatWindow::AlertID> blockedContactAlert_;

            MUCBookmarkManager* mucBookmarkManager_;
            boost::signals2::scoped_connection mucBookmarkManagerBookmarkAddedConnection_;
            boost::signals2::scoped_connection mucBookmarkManagerBookmarkRemovedConnection_;

            std::string subject_;
            bool isInitialJoin_;
            std::string chatWindowTitle_;
    };
}

