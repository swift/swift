/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Jingle/IncomingJingleSession.h>

namespace Swift {
	class IQRouter;
	class JingleResponder;
	class IncomingJingleSessionHandler;

	class JingleSessionManager {
			friend class JingleResponder;
		public:
			JingleSessionManager(IQRouter* router);
			~JingleSessionManager();

			JingleSession::ref getSession(const JID& jid, const std::string& id) const;

			void addIncomingSessionHandler(IncomingJingleSessionHandler* handler);
			void removeIncomingSessionHandler(IncomingJingleSessionHandler* handler);

		protected:
			void handleIncomingSession(const JID& from, IncomingJingleSession::ref);

		private:
			IQRouter* router;
			JingleResponder* responder;
			std::vector<IncomingJingleSessionHandler*> incomingSessionHandlers;
			struct JIDSession {
				JIDSession(const JID& jid, const std::string& session) : jid(jid), session(session) {}
				bool operator<(const JIDSession& o) const {
					return jid == o.jid ? session < o.session : jid < o.jid;
				}
				JID jid;
				std::string session;
			};
			typedef std::map<JIDSession, JingleSession::ref> SessionMap;
			SessionMap incomingSessions;
	};
}
