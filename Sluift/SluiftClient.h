/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/ClientError.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/PubSub/PubSubManager.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Roster/XMPPRosterItem.h>

#include <Sluift/Response.h>
#include <Sluift/Watchdog.h>

namespace Swift {
    struct SluiftGlobals;
    class ClientXMLTracer;
    class Client;
    class Stanza;
    class Payload;
    class ErrorPayload;
    class JID;

    class SluiftClient {
        public:
            struct Event {
                enum Type {
                    MessageType,
                    PresenceType,
                    PubSubEventType,
                    BlockEventType,
                    UnblockEventType
                };

                Event(std::shared_ptr<Message> stanza) : type(MessageType), stanza(stanza) {}
                Event(std::shared_ptr<Presence> stanza) : type(PresenceType), stanza(stanza) {}
                Event(const JID& from, std::shared_ptr<PubSubEventPayload> payload) : type(PubSubEventType), from(from), pubsubEvent(payload) {}
                Event(const JID& item, Type type) : type(type), item(item) {}
                Type type;

                // Message & Presence
                std::shared_ptr<Stanza> stanza;

                // PubSubEvent
                JID from;
                std::shared_ptr<PubSubEventPayload> pubsubEvent;

                // Blocklist
                JID item;
            };

            SluiftClient(
                    const JID& jid,
                    const std::string& password,
                    NetworkFactories* networkFactories,
                    SimpleEventLoop* eventLoop);
            ~SluiftClient();

            Client* getClient() {
                return client;
            }

            ClientOptions& getOptions() {
                return options;
            }

            void connect();
            void connect(const std::string& host, int port);
            void waitConnected(int timeout);
            bool isConnected() const;
            void setTraceEnabled(bool b);

            template<typename T>
                Sluift::Response sendPubSubRequest(
                    IQ::Type type, const JID& jid, std::shared_ptr<T> payload, int timeout) {
                return sendRequest(client->getPubSubManager()->createRequest(
                            type, jid, payload), timeout);
            }

            template<typename REQUEST_TYPE>
            Sluift::Response sendRequest(REQUEST_TYPE request, int timeout) {
                boost::signals2::scoped_connection c(request->onResponse.connect(
                        boost::bind(&SluiftClient::handleRequestResponse, this, _1, _2)));
                return doSendRequest(request, timeout);
            }

            template<typename REQUEST_TYPE>
            Sluift::Response sendVoidRequest(REQUEST_TYPE request, int timeout) {
                boost::signals2::scoped_connection c(request->onResponse.connect(
                        boost::bind(&SluiftClient::handleRequestResponse, this, std::shared_ptr<Payload>(), _1)));
                return doSendRequest(request, timeout);
            }

            void disconnect();
            void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os);
            boost::optional<SluiftClient::Event> getNextEvent(int timeout,
                    boost::function<bool (const Event&)> condition = 0);
            std::vector<XMPPRosterItem> getRoster(int timeout);
            std::vector<JID> getBlockList(int timeout);

        private:
            Sluift::Response doSendRequest(std::shared_ptr<Request> request, int timeout);

            void handleIncomingMessage(std::shared_ptr<Message> stanza);
            void handleIncomingPresence(std::shared_ptr<Presence> stanza);
            void handleIncomingPubSubEvent(const JID& from, std::shared_ptr<PubSubEventPayload> event);
            void handleIncomingBlockEvent(const JID& item);
            void handleIncomingUnblockEvent(const JID& item);
            void handleInitialRosterPopulated();
            void handleRequestResponse(std::shared_ptr<Payload> response, std::shared_ptr<ErrorPayload> error);
            void handleDisconnected(const boost::optional<ClientError>& error);

        private:
            NetworkFactories* networkFactories;
            SimpleEventLoop* eventLoop;
            Client* client;
            ClientOptions options;
            ClientXMLTracer* tracer;
            bool rosterReceived = false;
            bool blockListReceived = false;
            std::deque<Event> pendingEvents;
            boost::optional<ClientError> disconnectedError;
            bool requestResponseReceived = false;
            std::shared_ptr<Payload> requestResponse;
            std::shared_ptr<ErrorPayload> requestError;
    };
}
