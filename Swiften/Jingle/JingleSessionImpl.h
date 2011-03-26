/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Jingle/JingleSession.h>

namespace Swift {
	class JingleSessionImpl : public JingleSession {
			friend class JingleResponder;
		public:
			typedef boost::shared_ptr<JingleSessionImpl> ref;

			JingleSessionImpl(const JID& initiator, const std::string& id);

			virtual void terminate(JinglePayload::Reason::Type reason);
			virtual void accept(JingleTransportPayload::ref);
			virtual void sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref);
			virtual void acceptTransport(const JingleContentID&, JingleTransportPayload::ref);
			virtual void rejectTransport(const JingleContentID&, JingleTransportPayload::ref);

		private:
			void handleIncomingAction(JinglePayload::ref);
	};
}
