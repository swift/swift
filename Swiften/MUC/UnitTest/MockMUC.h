/*
 * Copyright (c) 2013-2016 Isode Limited.
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

    class SWIFTEN_API MockMUC : public MUC{
        public:
            typedef std::shared_ptr<MockMUC> ref;

        public:
            MockMUC(const JID &muc);
            virtual ~MockMUC();

            /**
             * Cause a user to appear to have entered the room. For testing only.
             */
            void insertOccupant(const MUCOccupant& occupant);

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
            virtual bool isUnlocked() const { return true; }

            virtual void joinAs(const std::string&) {}
            virtual void joinWithContextSince(const std::string&, const boost::posix_time::ptime&) {}
            /*virtual void queryRoomInfo(); */
            /*virtual void queryRoomItems(); */
            /*virtual std::string getCurrentNick() = 0; */
            virtual std::map<std::string, MUCOccupant> getOccupants() const { return occupants_; }
            virtual void changeNickname(const std::string&) { }
            virtual void part() {}
            /*virtual void handleIncomingMessage(Message::ref message) = 0; */
            /** Expose public so it can be called when e.g. user goes offline */
            virtual void handleUserLeft(LeavingType) {}
            /** Get occupant information*/
            virtual const MUCOccupant& getOccupant(const std::string&);
            virtual bool hasOccupant(const std::string&);
            virtual void kickOccupant(const JID&) {}
            virtual void changeOccupantRole(const JID&, MUCOccupant::Role);
            virtual void requestAffiliationList(MUCOccupant::Affiliation) {}
            virtual void changeAffiliation(const JID&, MUCOccupant::Affiliation);
            virtual void changeSubject(const std::string&);
            virtual void requestConfigurationForm() {}
            virtual void configureRoom(Form::ref) {}
            virtual void cancelConfigureRoom() {}
            virtual void destroyRoom() {}
            /** Send an invite for the person to join the MUC */
            virtual void invitePerson(const JID&, const std::string&, bool, bool) {}
            virtual void setCreateAsReservedIfNew() {}
            virtual void setPassword(const boost::optional<std::string>&) {}

        protected:
            virtual bool isFromMUC(const JID& j) const {
                return ownMUCJID.equals(j, JID::WithoutResource);
            }

            virtual const std::string& getOwnNick() const {
                return ownMUCJID.getResource();
            }

        private:
            JID ownMUCJID;
            std::map<std::string, MUCOccupant> occupants_;

        public:
            std::string newSubjectSet_;
    };
}
