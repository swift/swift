#ifndef SWIFTEN_MUC_H
#define SWIFTEN_MUC_H

#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/MUC/MUCOccupant.h"

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include <map>

namespace Swift {
	class StanzaChannel;

	class MUC {
		public:
			enum JoinResult { JoinSucceeded, JoinFailed };
			enum LeavingType { Part };

		public:
			MUC(StanzaChannel* stanzaChannel, const JID &muc);
			~MUC();

			void joinAs(const String &nick);
			String getCurrentNick();
			void part();
			void handleIncomingMessage(boost::shared_ptr<Message> message);

		public:
			boost::signal<void (JoinResult)> onJoinComplete;
			boost::signal<void (boost::shared_ptr<Message>)> onMessageReceived;
			boost::signal<void (boost::shared_ptr<Presence>)> onOccupantPresenceChange;
			boost::signal<void (const MUCOccupant&)> onOccupantJoined;
			/**Occupant, type, and reason. */
			boost::signal<void (const MUCOccupant&, LeavingType, const String&)> onOccupantLeft;

		private:
			void handleIncomingPresence(boost::shared_ptr<Presence> presence);
			JID muc_;
			StanzaChannel *stanzaChannel_;
			String myNick_;
			std::map<String, MUCOccupant> occupants_;
	};
}

#endif
