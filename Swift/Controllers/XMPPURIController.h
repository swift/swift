/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/signals2.hpp>

namespace Swift {
    class URIHandler;
    class JID;
    class UIEventStream;

    class XMPPURIController {
        public:
            XMPPURIController(URIHandler* uriHandler, UIEventStream* uiEventStream);
            ~XMPPURIController();

            boost::signals2::signal<void (const JID&)> onStartChat;
            boost::signals2::signal<void (const JID&)> onJoinMUC;

        private:
            void handleURI(const std::string&);

        private:
            URIHandler* uriHandler;
            UIEventStream* uiEventStream;
    };
}
