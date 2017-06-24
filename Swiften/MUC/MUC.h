/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/signals2.hpp>
#include <boost/signals2/connection.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUCRegistry.h>

namespace Swift {
    class SWIFTEN_API MUC {
        public:
            typedef std::shared_ptr<MUC> ref;

            enum JoinResult { JoinSucceeded, JoinFailed };
            enum LeavingType { LeavePart, LeaveKick, LeaveBan, LeaveDestroy, LeaveNotMember, Disconnect };

        public:
            virtual ~MUC();

            /**
             * Returns the (bare) JID of the MUC.
             */
            virtual JID getJID() const = 0;

            /**
             * Returns if the room is unlocked and other people can join the room.
             * @return True if joinable by others; false otherwise.
             */
            virtual bool isUnlocked() const = 0;

            virtual void joinAs(const std::string &nick) = 0;
            virtual void joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since) = 0;
            /*virtual void queryRoomInfo(); */
            /*virtual void queryRoomItems(); */
            /*virtual std::string getCurrentNick() = 0; */
            virtual std::map<std::string, MUCOccupant> getOccupants() const = 0;
            virtual void changeNickname(const std::string& newNickname) = 0;
            virtual void part() = 0;
            /*virtual void handleIncomingMessage(Message::ref message) = 0; */
            /** Expose public so it can be called when e.g. user goes offline */
            virtual void handleUserLeft(LeavingType) = 0;
            /** Get occupant information*/
            virtual const MUCOccupant& getOccupant(const std::string& nick) = 0;
            virtual bool hasOccupant(const std::string& nick) = 0;
            virtual void kickOccupant(const JID& jid) = 0;
            virtual void changeOccupantRole(const JID& jid, MUCOccupant::Role role) = 0;
            virtual void requestAffiliationList(MUCOccupant::Affiliation) = 0;
            virtual void changeAffiliation(const JID& jid, MUCOccupant::Affiliation affiliation) = 0;
            virtual void changeSubject(const std::string& subject) = 0;
            virtual void requestConfigurationForm() = 0;
            virtual void configureRoom(Form::ref) = 0;
            virtual void cancelConfigureRoom() = 0;
            virtual void destroyRoom() = 0;
            /** Send an invite for the person to join the MUC */
            virtual void invitePerson(const JID& person, const std::string& reason = "", bool isImpromptu = false, bool isReuseChat = false) = 0;
            virtual void setCreateAsReservedIfNew() = 0;
            virtual void setPassword(const boost::optional<std::string>& password) = 0;

        public:
            boost::signals2::signal<void (const std::string& /*nick*/)> onJoinComplete;
            boost::signals2::signal<void (ErrorPayload::ref)> onJoinFailed;
            boost::signals2::signal<void (ErrorPayload::ref, const JID&, MUCOccupant::Role)> onRoleChangeFailed;
            boost::signals2::signal<void (ErrorPayload::ref, const JID&, MUCOccupant::Affiliation)> onAffiliationChangeFailed;
            boost::signals2::signal<void (ErrorPayload::ref)> onConfigurationFailed;
            boost::signals2::signal<void (ErrorPayload::ref)> onAffiliationListFailed;
            boost::signals2::signal<void (Presence::ref)> onOccupantPresenceChange;
            boost::signals2::signal<void (const std::string&, const MUCOccupant& /*now*/, const MUCOccupant::Role& /*old*/)> onOccupantRoleChanged;
            boost::signals2::signal<void (const std::string&, const MUCOccupant::Affiliation& /*new*/, const MUCOccupant::Affiliation& /*old*/)> onOccupantAffiliationChanged;
            boost::signals2::signal<void (const MUCOccupant&)> onOccupantJoined;
            boost::signals2::signal<void (const std::string& /*oldNickname*/, const std::string& /*newNickname*/ )> onOccupantNicknameChanged;
            boost::signals2::signal<void (const MUCOccupant&, LeavingType, const std::string& /*reason*/)> onOccupantLeft;
            boost::signals2::signal<void (Form::ref)> onConfigurationFormReceived;
            boost::signals2::signal<void (MUCOccupant::Affiliation, const std::vector<JID>&)> onAffiliationListReceived;
            boost::signals2::signal<void ()> onUnlocked;
            /* boost::signals2::signal<void (const MUCInfo&)> onInfoResult; */
            /* boost::signals2::signal<void (const blah&)> onItemsResult; */

    };
}
