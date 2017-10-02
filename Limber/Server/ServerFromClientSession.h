/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Session/Session.h>

namespace Swift {
    class ProtocolHeader;
    class ToplevelElement;
    class Stanza;
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;
    class StreamStack;
    class UserRegistry;
    class XMPPLayer;
    class ConnectionLayer;
    class Connection;
    class XMLParserFactory;

    class ServerFromClientSession : public Session {
        public:
            ServerFromClientSession(
                    const std::string& id,
                    std::shared_ptr<Connection> connection,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    XMLParserFactory* xmlParserFactory,
                    UserRegistry* userRegistry);

            boost::signals2::signal<void ()> onSessionStarted;
            void setAllowSASLEXTERNAL();

        private:
            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamStart(const ProtocolHeader& header);

            void setInitialized();
            bool isInitialized() const {
                return initialized;
            }

        private:
            std::string id_;
            UserRegistry* userRegistry_;
            bool authenticated_;
            bool initialized;
            bool allowSASLEXTERNAL;
            std::string user_;
    };
}
