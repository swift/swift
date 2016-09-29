/*
 * Copyright (c) 2010-2016 Isode Limited.
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
#include <Swiften/MUC/MUC.h>
#include <Swiften/MUC/MUCRegistry.h>

namespace Swift {
    class StanzaChannel;
    class IQRouter;
    class DirectedPresenceSender;

    class SWIFTEN_API MUCImpl : public MUC {
        public:
            typedef std::shared_ptr<MUCImpl> ref;

        public:
            MUCImpl(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry);
            virtual ~MUCImpl();

            /**
             * Returns the (bare) JID of the MUC.
             */
            virtual JID getJID() const {
                return ownMUCJID.toBare();
            }

            /**
             * Returns if the room is unlocked and other people can join the room.
             * @return True if joinable by others; false otherwise.
             */
            virtual bool isUnlocked() const {
                return isUnlocked_;
            }

            virtual void joinAs(const std::string &nick);
            virtual void joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since);
            /*virtual void queryRoomInfo(); */
            /*virtual void queryRoomItems(); */
            /*virtual std::string getCurrentNick(); */
            virtual std::map<std::string, MUCOccupant> getOccupants() const;

            /**
             * Send a new presence to the MUC indicating a nickname change. Any custom status the user had in the is cleared.
             * @param newNickname The nickname to change to.
             */
            virtual void changeNickname(const std::string& newNickname);
            virtual void part();
            /*virtual void handleIncomingMessage(Message::ref message); */
            /** Expose public so it can be called when e.g. user goes offline */
            virtual void handleUserLeft(LeavingType);
            /** Get occupant information*/
            virtual const MUCOccupant& getOccupant(const std::string& nick);
            virtual bool hasOccupant(const std::string& nick);
            virtual void kickOccupant(const JID& jid);
            virtual void changeOccupantRole(const JID& jid, MUCOccupant::Role role);
            virtual void requestAffiliationList(MUCOccupant::Affiliation);
            virtual void changeAffiliation(const JID& jid, MUCOccupant::Affiliation affiliation);
            virtual void changeSubject(const std::string& subject);
            virtual void requestConfigurationForm();
            virtual void configureRoom(Form::ref);
            virtual void cancelConfigureRoom();
            virtual void destroyRoom();
            /** Send an invite for the person to join the MUC */
            virtual void invitePerson(const JID& person, const std::string& reason = "", bool isImpromptu = false, bool isReuseChat = false);
            virtual void setCreateAsReservedIfNew() {createAsReservedIfNew = true;}
            virtual void setPassword(const boost::optional<std::string>& password);

        private:
            bool isFromMUC(const JID& j) const {
                return ownMUCJID.equals(j, JID::WithoutResource);
            }

            const std::string& getOwnNick() const {
                return ownMUCJID.getResource();
            }

            /**
             * This function compares two Presence elements for equality based on to, from, status, show and entity capability information.
             * @return True if equal; else otherwise.
             */
            static bool isEqualExceptID(const Presence& lhs, const Presence& rhs);

        private:
            void handleIncomingPresence(Presence::ref presence);
            void internalJoin(const std::string& nick);
            void handleCreationConfigResponse(MUCOwnerPayload::ref, ErrorPayload::ref);
            void handleOccupantRoleChangeResponse(MUCAdminPayload::ref, ErrorPayload::ref, const JID&, MUCOccupant::Role);
            void handleAffiliationChangeResponse(MUCAdminPayload::ref, ErrorPayload::ref, const JID&, MUCOccupant::Affiliation);
            void handleAffiliationListResponse(MUCAdminPayload::ref, ErrorPayload::ref, MUCOccupant::Affiliation);
            void handleConfigurationFormReceived(MUCOwnerPayload::ref, ErrorPayload::ref);
            void handleConfigurationResultReceived(MUCOwnerPayload::ref, ErrorPayload::ref);

        private:
            JID ownMUCJID;
            StanzaChannel* stanzaChannel;
            IQRouter* iqRouter_;
            DirectedPresenceSender* presenceSender;
            MUCRegistry* mucRegistry;
            std::map<std::string, MUCOccupant> occupants;
            bool joinSucceeded_ = false;
            bool joinComplete_ = false;
            boost::signals2::scoped_connection scopedConnection_;
            boost::posix_time::ptime joinSince_;
            bool createAsReservedIfNew = false;
            bool unlocking = false;
            bool isUnlocked_ = false;
            boost::optional<std::string> password;
            Presence::ref joinRequestPresence_;
    };
}
