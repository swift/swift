/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class URIHandler;
	class JID;
	class UIEventStream;

	class XMPPURIController {
		public:
			XMPPURIController(URIHandler* uriHandler, UIEventStream* uiEventStream);
			~XMPPURIController();

			boost::signal<void (const JID&)> onStartChat;
			boost::signal<void (const JID&)> onJoinMUC;

		private:
			void handleURI(const std::string&);

		private:
			URIHandler* uriHandler;
			UIEventStream* uiEventStream;
	};
}
