/*
 * Copyright (c) 2011-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
	class IQRouter;
	class Request;

	class JingleSessionImpl : public JingleSession {
			friend class JingleResponder;
		public:
			typedef boost::shared_ptr<JingleSessionImpl> ref;

			JingleSessionImpl(const JID& initiator, const JID&, const std::string& id, IQRouter* router);

			virtual void sendInitiate(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref);
			virtual void sendTerminate(JinglePayload::Reason::Type reason);
			virtual void sendInfo(boost::shared_ptr<Payload>);
			virtual void sendAccept(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref);
			virtual std::string sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportAccept(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportReject(const JingleContentID&, JingleTransportPayload::ref);
			virtual void sendTransportReplace(const JingleContentID&, JingleTransportPayload::ref);

		private:
			typedef boost::shared_ptr<GenericRequest<JinglePayload> > RequestRef;

			void handleIncomingAction(JinglePayload::ref);
			
			std::string sendSetRequest(JinglePayload::ref payload);
			JinglePayload::ref createPayload() const;
			void handleRequestResponse(RequestRef);
			
		private:
			IQRouter *iqRouter;
			JID peerJID;
			typedef std::map<RequestRef, boost::bsignals::connection > RequestsMap;
			RequestsMap pendingRequests;
	};
}
