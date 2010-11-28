/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/MUC/MUCOccupant.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Elements/MUCOwnerPayload.h"

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"
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
			enum LeavingType { Part, Disconnect };

		public:
			MUC(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry);

			/**
			 * Returns the (bare) JID of the MUC.
			 */
			JID getJID() const {
				return ownMUCJID.toBare();
			}

			void joinAs(const String &nick);
			void joinWithContextSince(const String &nick, const boost::posix_time::ptime& since);
			/*void queryRoomInfo(); */
			/*void queryRoomItems(); */
			String getCurrentNick();
			void part();
			void handleIncomingMessage(boost::shared_ptr<Message> message);
			/** Expose public so it can be called when e.g. user goes offline */
			void handleUserLeft(LeavingType);

		public:
			boost::signal<void (const String& /*nick*/)> onJoinComplete;
			boost::signal<void (boost::shared_ptr<ErrorPayload>)> onJoinFailed;
			boost::signal<void (boost::shared_ptr<Presence>)> onOccupantPresenceChange;
			boost::signal<void (const String&, const MUCOccupant& /*now*/, const MUCOccupant::Role& /*old*/)> onOccupantRoleChanged;
			boost::signal<void (const String&, const MUCOccupant::Affiliation& /*new*/, const MUCOccupant::Affiliation& /*old*/)> onOccupantAffiliationChanged;
			boost::signal<void (const MUCOccupant&)> onOccupantJoined;
			boost::signal<void (const MUCOccupant&, LeavingType, const String& /*reason*/)> onOccupantLeft;
			/* boost::signal<void (const MUCInfo&)> onInfoResult; */
			/* boost::signal<void (const blah&)> onItemsResult; */
			

		private:
			bool isFromMUC(const JID& j) const {
				return ownMUCJID.equals(j, JID::WithoutResource);
			}

			const String& getOwnNick() const {
				return ownMUCJID.getResource();
			}

		private:
			void handleIncomingPresence(boost::shared_ptr<Presence> presence);
			void internalJoin(const String& nick);
			void handleCreationConfigResponse(boost::shared_ptr<MUCOwnerPayload>, ErrorPayload::ref);

		private:
			JID ownMUCJID;
			StanzaChannel* stanzaChannel;
			IQRouter* iqRouter_;
			DirectedPresenceSender* presenceSender;
			MUCRegistry* mucRegistry;
			std::map<String, MUCOccupant> occupants;
			bool joinComplete_;
			boost::bsignals::scoped_connection scopedConnection_;
			boost::posix_time::ptime joinSince_;
	};
}
