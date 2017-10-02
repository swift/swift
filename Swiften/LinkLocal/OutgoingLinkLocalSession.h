/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Session/Session.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;
    class ToplevelElement;
    class XMLParserFactory;

    class SWIFTEN_API OutgoingLinkLocalSession : public Session {
        public:
            OutgoingLinkLocalSession(
                    const JID& localJID,
                    const JID& remoteJID,
                    std::shared_ptr<Connection> connection,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    XMLParserFactory* xmlParserFactory);

            void queueElement(std::shared_ptr<ToplevelElement> element);

        private:
            void handleSessionStarted();
            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamStart(const ProtocolHeader&);

        private:
            std::vector<std::shared_ptr<ToplevelElement> > queuedElements_;
    };
}
