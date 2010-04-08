/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/MUC/MUCOccupant.h"

#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>

#include <map>

namespace Swift {
	class StanzaChannel;
	class PresenceSender;

	class MUC {
		public:
			enum JoinResult { JoinSucceeded, JoinFailed };
			enum LeavingType { Part };

		public:
			MUC(StanzaChannel* stanzaChannel, PresenceSender* presenceSender, const JID &muc);

			void joinAs(const String &nick);
			String getCurrentNick();
			void part();
			void handleIncomingMessage(boost::shared_ptr<Message> message);

		public:
			boost::signal<void (JoinResult)> onJoinComplete;
			boost::signal<void (boost::shared_ptr<Presence>)> onOccupantPresenceChange;
			boost::signal<void (const MUCOccupant&)> onOccupantJoined;
			/**Occupant, type, and reason. */
			boost::signal<void (const MUCOccupant&, LeavingType, const String&)> onOccupantLeft;

		private:
			bool isFromMUC(const JID& j) const {
				return ownMUCJID.equals(j, JID::WithoutResource);
			}

			const String& getOwnNick() const {
				return ownMUCJID.getResource();
			}

		private:
			void handleIncomingPresence(boost::shared_ptr<Presence> presence);

		private:
			JID ownMUCJID;
			StanzaChannel* stanzaChannel;
			PresenceSender* presenceSender;
			std::map<String, MUCOccupant> occupants;
			bool firstPresenceSeen;
			boost::bsignals::scoped_connection scopedConnection_;
	};
}
