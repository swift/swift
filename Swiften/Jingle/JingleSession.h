/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

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

			const std::string& getID() const {
				return id;
			}
			virtual void sendInitiate(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref) = 0;
			virtual void sendTerminate(JinglePayload::Reason::Type reason) = 0;
			virtual void sendInfo(boost::shared_ptr<Payload>) = 0;
			virtual void sendAccept(const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref = JingleTransportPayload::ref()) = 0;
			virtual void sendTransportInfo(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportAccept(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportReject(const JingleContentID&, JingleTransportPayload::ref) = 0;
			virtual void sendTransportReplace(const JingleContentID&, JingleTransportPayload::ref) = 0;

		public:
			boost::signal<void (const JingleContentID&, JingleDescription::ref, JingleTransportPayload::ref)> onSessionAcceptReceived;
			boost::signal<void (JinglePayload::ref)> onSessionInfoReceived;
			boost::signal<void (boost::optional<JinglePayload::Reason>)> onSessionTerminateReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportAcceptReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportInfoReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportRejectReceived;
			boost::signal<void (const JingleContentID&, JingleTransportPayload::ref)> onTransportReplaceReceived;

		private:
			JID initiator;
			std::string id;
	};
}
