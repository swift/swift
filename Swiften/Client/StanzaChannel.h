#ifndef SWIFTEN_MessageChannel_H
#define SWIFTEN_MessageChannel_H

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Queries/IQChannel.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"

namespace Swift {
	class StanzaChannel : public IQChannel {
		public:
			virtual void sendMessage(boost::shared_ptr<Message>) = 0;
			virtual void sendPresence(boost::shared_ptr<Presence>) = 0;
			virtual bool isAvailable() = 0;

			boost::signal<void (boost::shared_ptr<Message>)> onMessageReceived;
			boost::signal<void (boost::shared_ptr<Presence>) > onPresenceReceived;
	};
}

#endif
