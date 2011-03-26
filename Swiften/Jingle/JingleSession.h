/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/JinglePayload.h>

namespace Swift {
	class JingleContentID;

	class JingleSession {
		public:
			typedef boost::shared_ptr<JingleSession> ref;

			JingleSession(const JID& initiator, const std::string& id);
			virtual ~JingleSession();

			const JID& getInitiator() const {
				return initiator;
			}

			std::string getID() const {
				return id;
			}

			virtual void terminate(JinglePayload::Reason::Type reason) = 0;
			virtual void accept(JingleTransportPayload::ref = JingleTransportPayload::ref()) = 0;
			virtual void sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void acceptTransport(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void rejectTransport(const JingleContentID&, JingleTransportPayload::ref) = 0;

		public:
			boost::signal<void ()> onSessionTerminateReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportInfoReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportReplaceReceived;

		private:
			JID initiator;
			std::string id;
	};
}
