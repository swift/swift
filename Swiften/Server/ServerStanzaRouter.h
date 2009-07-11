#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Stanza.h"

namespace Swift {
	class ServerSession;

	class ServerStanzaRouter {
		public:
			ServerStanzaRouter();

			bool routeStanza(boost::shared_ptr<Stanza>);

			void addClientSession(ServerSession*);
			void removeClientSession(ServerSession*);

		private:
			std::vector<ServerSession*> clientSessions_;
	};
}
