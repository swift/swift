/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Component/Component.h>
#include <Swiften/Component/ComponentError.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Roster/XMPPRosterItem.h>

#include <Sluift/Response.h>
#include <Sluift/Watchdog.h>

namespace Swift {
    struct SluiftGlobals;
    class ComponentXMLTracer;
    class Component;
    class Stanza;
    class Payload;
    class ErrorPayload;
    class JID;

    class SluiftComponent {
        public:
            struct Event {
                enum Type {
                    MessageType,
                    PresenceType
                };

                Event(std::shared_ptr<Message> stanza) : type(MessageType), stanza(stanza) {}
                Event(std::shared_ptr<Presence> stanza) : type(PresenceType), stanza(stanza) {}

                Type type;

                // Message & Presence
                std::shared_ptr<Stanza> stanza;
            };

            SluiftComponent(
                    const JID& jid,
                    const std::string& password,
                    NetworkFactories* networkFactories,
                    SimpleEventLoop* eventLoop);
            ~SluiftComponent();

            Component* getComponent() {
                return component;
            }

            void connect(const std::string& host, int port);
            void waitConnected(int timeout);
            bool isConnected() const;
            void setTraceEnabled(bool b);

            template<typename REQUEST_TYPE>
            Sluift::Response sendRequest(REQUEST_TYPE request, int timeout) {
                boost::signals2::scoped_connection c(request->onResponse.connect(
                        boost::bind(&SluiftComponent::handleRequestResponse, this, _1, _2)));
                return doSendRequest(request, timeout);
            }

            template<typename REQUEST_TYPE>
            Sluift::Response sendVoidRequest(REQUEST_TYPE request, int timeout) {
                boost::signals2::scoped_connection c(request->onResponse.connect(
                        boost::bind(&SluiftComponent::handleRequestResponse, this, std::shared_ptr<Payload>(), _1)));
                return doSendRequest(request, timeout);
            }

            void disconnect();
            void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os);
            boost::optional<SluiftComponent::Event> getNextEvent(int timeout,
                    boost::function<bool (const Event&)> condition = 0);

        private:
            Sluift::Response doSendRequest(std::shared_ptr<Request> request, int timeout);

            void handleIncomingMessage(std::shared_ptr<Message> stanza);
            void handleIncomingPresence(std::shared_ptr<Presence> stanza);
            void handleRequestResponse(std::shared_ptr<Payload> response, std::shared_ptr<ErrorPayload> error);
            void handleError(const boost::optional<ComponentError>& error);

        private:
            NetworkFactories* networkFactories;
            SimpleEventLoop* eventLoop;
            Component* component;
            ComponentXMLTracer* tracer;
            std::deque<Event> pendingEvents;
            boost::optional<ComponentError> disconnectedError;
            bool requestResponseReceived = false;
            std::shared_ptr<Payload> requestResponse;
            std::shared_ptr<ErrorPayload> requestError;
    };
}
