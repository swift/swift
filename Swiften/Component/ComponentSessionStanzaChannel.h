/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	/**
	 * StanzaChannel implementation around a ComponentSession.
	 */
	class ComponentSessionStanzaChannel : public StanzaChannel {
		public:
			void setSession(boost::shared_ptr<ComponentSession> session);

			void sendIQ(boost::shared_ptr<IQ> iq);
			void sendMessage(boost::shared_ptr<Message> message);
			void sendPresence(boost::shared_ptr<Presence> presence);

			bool getStreamManagementEnabled() const {
				return false;
			}

			bool isAvailable() const {
				return session && session->getState() == ComponentSession::Initialized;
			}

		private:
			std::string getNewIQID();
			void send(boost::shared_ptr<Stanza> stanza);
			void handleSessionFinished(boost::shared_ptr<Error> error);
			void handleStanza(boost::shared_ptr<Stanza> stanza);
			void handleSessionInitialized();

		private:
			IDGenerator idGenerator;
			boost::shared_ptr<ComponentSession> session;
	};

}
