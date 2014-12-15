/*
 * Copyright (c) 2011-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/Listenable.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/JinglePayload.h>

namespace Swift {
	class JingleSessionListener;
	class JingleContentID;

	class JingleSession : public Listenable<JingleSessionListener> {
		public:
			typedef boost::shared_ptr<JingleSession> ref;

			JingleSession(const JID& initiator, const std::string& id);
			virtual ~JingleSession();

			const JID& getInitiator() const {
				return initiator;
			}

			const std::string& getID() const {
				return id;
			}

			virtual void sendInitiate(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref) = 0;
			virtual void sendTerminate(JinglePayload::Reason::Type reason) = 0;
			virtual void sendInfo(boost::shared_ptr<Payload>) = 0;
			virtual void sendAccept(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref = JingleTransportPayload::ref()) = 0;
			virtual std::string sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportAccept(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportReject(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportReplace(const JingleContentID&, JingleTransportPayload::ref) = 0;

		private:
			JID initiator;
			std::string id;
			std::vector<JingleSessionListener*> listeners;
	};
}
