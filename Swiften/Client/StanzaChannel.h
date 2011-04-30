/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Queries/IQChannel.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	class StanzaChannel : public IQChannel {
		public:
			virtual void sendMessage(boost::shared_ptr<Message>) = 0;
			virtual void sendPresence(boost::shared_ptr<Presence>) = 0;
			virtual bool isAvailable() const = 0;
			virtual bool getStreamManagementEnabled() const = 0;

			boost::signal<void (bool /* isAvailable */)> onAvailableChanged;
			boost::signal<void (boost::shared_ptr<Message>)> onMessageReceived;
			boost::signal<void (boost::shared_ptr<Presence>) > onPresenceReceived;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaAcked;
	};
}
