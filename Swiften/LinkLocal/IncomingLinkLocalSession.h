/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Session/Session.h>

namespace Swift {
    class ProtocolHeader;
    class XMLParserFactory;
    class ToplevelElement;
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;

    class SWIFTEN_API IncomingLinkLocalSession : public Session {
        public:
            IncomingLinkLocalSession(
                    const JID& localJID,
                    std::shared_ptr<Connection> connection,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    XMLParserFactory* xmlParserFactory);

            boost::signal<void ()> onSessionStarted;

        private:
            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamStart(const ProtocolHeader&);
            void setInitialized();
            bool isInitialized() const {
                return initialized;
            }

            bool initialized;
    };
}
