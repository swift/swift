/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <string>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Elements/Form.h>

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/signals/connection.hpp>

#include <map>

namespace Swift {
	class StanzaChannel;
	class IQRouter;
	class DirectedPresenceSender;

	class MUC {
		public:
			typedef boost::shared_ptr<MUC> ref;

			enum JoinResult { JoinSucceeded, JoinFailed };
			enum LeavingType { LeavePart, LeaveKick, LeaveBan, LeaveDestroy, LeaveNotMember, Disconnect };

		public:
			MUC(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry);

			/**
			 * Returns the (bare) JID of the MUC.
			 */
			JID getJID() const {
				return ownMUCJID.toBare();
			}

			void joinAs(const std::string &nick);
			void joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since);
			/*void queryRoomInfo(); */
			/*void queryRoomItems(); */
			std::string getCurrentNick();
			void part();
			void handleIncomingMessage(Message::ref message);
			/** Expose public so it can be called when e.g. user goes offline */
			void handleUserLeft(LeavingType);
			/** Get occupant information*/
			const MUCOccupant& getOccupant(const std::string& nick);
			bool hasOccupant(const std::string& nick);
			void kickOccupant(const JID& jid);
			void changeOccupantRole(const JID& jid, MUCOccupant::Role role);
			void requestAffiliationList(MUCOccupant::Affiliation);
			void changeAffiliation(const JID& jid, MUCOccupant::Affiliation affiliation);
			void changeSubject(const std::string& subject);
			void requestConfigurationForm();
			void configureRoom(Form::ref);
			void cancelConfigureRoom();
			void destroyRoom();
			/** Send an invite for the person to join the MUC */
			void invitePerson(const JID& person, const std::string& reason = "");
			void setCreateAsReservedIfNew() {createAsReservedIfNew = true;}
			void setPassword(const boost::optional<std::string>& password);
			
		public:
			boost::signal<void (const std::string& /*nick*/)> onJoinComplete;
			boost::signal<void (ErrorPayload::ref)> onJoinFailed;
			boost::signal<void (ErrorPayload::ref, const JID&, MUCOccupant::Role)> onRoleChangeFailed;
			boost::signal<void (ErrorPayload::ref, const JID&, MUCOccupant::Affiliation)> onAffiliationChangeFailed;
			boost::signal<void (ErrorPayload::ref)> onConfigurationFailed;
			boost::signal<void (ErrorPayload::ref)> onAffiliationListFailed;
			boost::signal<void (Presence::ref)> onOccupantPresenceChange;
			boost::signal<void (const std::string&, const MUCOccupant& /*now*/, const MUCOccupant::Role& /*old*/)> onOccupantRoleChanged;
			boost::signal<void (const std::string&, const MUCOccupant::Affiliation& /*new*/, const MUCOccupant::Affiliation& /*old*/)> onOccupantAffiliationChanged;
			boost::signal<void (const MUCOccupant&)> onOccupantJoined;
			boost::signal<void (const MUCOccupant&, LeavingType, const std::string& /*reason*/)> onOccupantLeft;
			boost::signal<void (Form::ref)> onConfigurationFormReceived;
			boost::signal<void (MUCOccupant::Affiliation, const std::vector<JID>&)> onAffiliationListReceived;
			/* boost::signal<void (const MUCInfo&)> onInfoResult; */
			/* boost::signal<void (const blah&)> onItemsResult; */
			

		private:
			bool isFromMUC(const JID& j) const {
				return ownMUCJID.equals(j, JID::WithoutResource);
			}

			const std::string& getOwnNick() const {
				return ownMUCJID.getResource();
			}

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
			bool joinSucceeded_;
			bool joinComplete_;
			boost::bsignals::scoped_connection scopedConnection_;
			boost::posix_time::ptime joinSince_;
			bool createAsReservedIfNew;
			bool unlocking;
			boost::optional<std::string> password;
	};
}
