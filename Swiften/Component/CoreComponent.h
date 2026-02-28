/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Component/ComponentConnector.h>
#include <Swiften/Component/ComponentError.h>
#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Component/ComponentSessionStanzaChannel.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Entity/Entity.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

namespace Swift {
    class BasicSessionStream;
    class ComponentSession;
    class IQRouter;
    class NetworkFactories;

    /**
     * The central class for communicating with an XMPP server as a component.
     *
     * This class is responsible for setting up the connection with the XMPP
     * server and authenticating the component.
     *
     * This class can be used directly in your application, although the Component
     * subclass provides more functionality and interfaces, and is better suited
     * for most needs.
     */
    class SWIFTEN_API CoreComponent : public Entity {
        public:
            CoreComponent(const JID& jid, const std::string& secret, NetworkFactories* networkFactories);
            virtual ~CoreComponent();

            void connect(const std::string& host, unsigned short port);
            void disconnect();

            void sendMessage(std::shared_ptr<Message>);
            void sendPresence(std::shared_ptr<Presence>);
            void sendData(const std::string& data);

            IQRouter* getIQRouter() const {
                return iqRouter_;
            }

            StanzaChannel* getStanzaChannel() const {
                return stanzaChannel_;
            }

            /**
             * Checks whether the component is connected to the server,
             * and stanzas can be sent.
             */
            bool isAvailable() const;

            /**
             * Checks whether the component is active.
             *
             * A component is active when it is connected or connecting to the server.
             */
            bool isActive() const;

            /**
             * Returns the JID of the component
             */
            const JID& getJID() const {
                return jid_;
            }

        public:
            boost::signals2::signal<void (const ComponentError&)> onError;
            boost::signals2::signal<void ()> onConnected;
            boost::signals2::signal<void (const SafeByteArray&)> onDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onDataWritten;

            boost::signals2::signal<void (std::shared_ptr<Message>)> onMessageReceived;
            boost::signals2::signal<void (std::shared_ptr<Presence>) > onPresenceReceived;

        private:
            void handleConnectorFinished(std::shared_ptr<Connection>);
            void handleStanzaChannelAvailableChanged(bool available);
            void handleSessionFinished(std::shared_ptr<Error>);
            void handleDataRead(const SafeByteArray&);
            void handleDataWritten(const SafeByteArray&);

        private:
            NetworkFactories* networkFactories;
            JID jid_;
            std::string secret_;
            ComponentSessionStanzaChannel* stanzaChannel_;
            IQRouter* iqRouter_;
            ComponentConnector::ref connector_;
            std::shared_ptr<Connection> connection_;
            std::shared_ptr<BasicSessionStream> sessionStream_;
            std::shared_ptr<ComponentSession> session_;
            bool disconnectRequested_;
    };
}
