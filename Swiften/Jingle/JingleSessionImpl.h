/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Jingle/JingleSession.h>

namespace Swift {
	class IQRouter;

	class JingleSessionImpl : public JingleSession {
			friend class JingleResponder;
		public:
			typedef boost::shared_ptr<JingleSessionImpl> ref;

			JingleSessionImpl(const JID& initiator, const JID&, const std::string& id, IQRouter* router);

			virtual void sendInitiate(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref);
			virtual void sendTerminate(JinglePayload::Reason::Type reason);
			virtual void sendInfo(boost::shared_ptr<Payload>);
			virtual void sendAccept(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref);
			virtual void sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportAccept(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportReject(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportReplace(const JingleContentID&, JingleTransportPayload::ref);

		private:
			void handleIncomingAction(JinglePayload::ref);
			
			void sendSetRequest(JinglePayload::ref payload);
			JinglePayload::ref createPayload() const;
			
		private:
			IQRouter *iqRouter;
			JID peerJID;
	};
}
