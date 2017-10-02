/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/SluiftClient.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/PubSubEvent.h>
#include <Swiften/Queries/RawRequest.h>
#include <Swiften/Roster/XMPPRoster.h>

#include <Sluift/Helpers.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/SluiftGlobals.h>

using namespace Swift;

SluiftClient::SluiftClient(
        const JID& jid,
        const std::string& password,
        NetworkFactories* networkFactories,
        SimpleEventLoop* eventLoop) :
            networkFactories(networkFactories),
            eventLoop(eventLoop),
            tracer(nullptr) {
    client = new Client(jid, password, networkFactories);
    client->setAlwaysTrustCertificates();
    client->onDisconnected.connect(boost::bind(&SluiftClient::handleDisconnected, this, _1));
    client->onMessageReceived.connect(boost::bind(&SluiftClient::handleIncomingMessage, this, _1));
    client->onPresenceReceived.connect(boost::bind(&SluiftClient::handleIncomingPresence, this, _1));
    client->getPubSubManager()->onEvent.connect(boost::bind(&SluiftClient::handleIncomingPubSubEvent, this, _1, _2));
    client->getRoster()->onInitialRosterPopulated.connect(boost::bind(&SluiftClient::handleInitialRosterPopulated, this));
    client->getClientBlockListManager()->getBlockList()->onItemAdded.connect(boost::bind(&SluiftClient::handleIncomingBlockEvent, this, _1));
    client->getClientBlockListManager()->getBlockList()->onItemRemoved.connect(boost::bind(&SluiftClient::handleIncomingUnblockEvent, this, _1));
}

SluiftClient::~SluiftClient() {
    delete tracer;
    delete client;
}

void SluiftClient::connect() {
    rosterReceived = false;
    blockListReceived = false;
    disconnectedError = boost::optional<ClientError>();
    client->connect(options);
}

void SluiftClient::setTraceEnabled(bool b) {
    if (b && !tracer) {
        tracer = new ClientXMLTracer(client, options.boshURL.isEmpty()? false: true);
    }
    else if (!b && tracer) {
        delete tracer;
        tracer = nullptr;
    }
}

void SluiftClient::waitConnected(int timeout) {
    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    while (!watchdog.getTimedOut() && client->isActive() && !client->isAvailable()) {
        eventLoop->runUntilEvents();
    }
    if (watchdog.getTimedOut()) {
        client->disconnect();
        throw Lua::Exception("Timeout while connecting");
    }
    if (disconnectedError) {
        throw Lua::Exception(getErrorString(*disconnectedError));
    }
}

bool SluiftClient::isConnected() const {
    return client->isAvailable();
}

void SluiftClient::disconnect() {
    client->disconnect();
    while (client->isActive()) {
        eventLoop->runUntilEvents();
    }
}

void SluiftClient::setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os) {
    client->setSoftwareVersion(name, version, os);
}

boost::optional<SluiftClient::Event> SluiftClient::getNextEvent(
        int timeout, boost::function<bool (const Event&)> condition) {
    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    size_t currentIndex = 0;
    while (true) {
        // Look for pending events in the queue
        while (currentIndex < pendingEvents.size()) {
            Event event = pendingEvents[currentIndex];
            if (!condition || condition(event)) {
                pendingEvents.erase(
                        pendingEvents.begin()
                        + boost::numeric_cast<int>(currentIndex));
                return event;
            }
            ++currentIndex;
        }

        // Wait for new events
        while (!watchdog.getTimedOut() && currentIndex >= pendingEvents.size() && client->isActive()) {
            eventLoop->runUntilEvents();
        }

        // Finish if we're disconnected or timed out
        if (watchdog.getTimedOut() || !client->isActive()) {
            return boost::optional<Event>();
        }
    }
}

std::vector<JID> SluiftClient::getBlockList(int timeout) {
    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    if (!blockListReceived) {
        // If we haven't requested it yet, request it for the first time
        client->getClientBlockListManager()->requestBlockList();

        // Wait for new events
        while (!watchdog.getTimedOut() && client->getClientBlockListManager()->getBlockList()->getState() != BlockList::Available) {
            eventLoop->runUntilEvents();
        }

        // Throw an error if we're timed out
        if (watchdog.getTimedOut()) {
            throw Lua::Exception("Timeout while requesting blocklist");
        }
    }
    blockListReceived = true;
    return client->getClientBlockListManager()->getBlockList()->getItems();
}

std::vector<XMPPRosterItem> SluiftClient::getRoster(int timeout) {
    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    if (!rosterReceived) {
        // If we haven't requested it yet, request it for the first time
        client->requestRoster();

        // Wait for new events
        while (!watchdog.getTimedOut() && !rosterReceived) {
            eventLoop->runUntilEvents();
        }

        // Throw an error if we're timed out
        if (watchdog.getTimedOut()) {
            throw Lua::Exception("Timeout while requesting roster");
        }
    }
    return client->getRoster()->getItems();
}

void SluiftClient::handleIncomingMessage(std::shared_ptr<Message> stanza) {
    if (stanza->getPayload<PubSubEvent>()) {
        // Already handled by pubsub manager
        return;
    }
    pendingEvents.push_back(Event(stanza));
}

void SluiftClient::handleIncomingPresence(std::shared_ptr<Presence> stanza) {
    pendingEvents.push_back(Event(stanza));
}

void SluiftClient::handleIncomingPubSubEvent(const JID& from, std::shared_ptr<PubSubEventPayload> event) {
    pendingEvents.push_back(Event(from, event));
}

void SluiftClient::handleIncomingBlockEvent(const JID& item) {
    pendingEvents.push_back(Event(item, Event::BlockEventType));
}

void SluiftClient::handleIncomingUnblockEvent(const JID& item) {
    pendingEvents.push_back(Event(item, Event::UnblockEventType));
}

void SluiftClient::handleInitialRosterPopulated() {
    rosterReceived = true;
}

void SluiftClient::handleRequestResponse(std::shared_ptr<Payload> response, std::shared_ptr<ErrorPayload> error) {
    requestResponse = response;
    requestError = error;
    requestResponseReceived = true;
}

void SluiftClient::handleDisconnected(const boost::optional<ClientError>& error) {
    disconnectedError = error;
}

Sluift::Response SluiftClient::doSendRequest(std::shared_ptr<Request> request, int timeout) {
    requestResponse.reset();
    requestError.reset();
    requestResponseReceived = false;
    request->send();

    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    while (!watchdog.getTimedOut() && !requestResponseReceived) {
        eventLoop->runUntilEvents();
    }
    return Sluift::Response(requestResponse, watchdog.getTimedOut() ?
            std::make_shared<ErrorPayload>(ErrorPayload::RemoteServerTimeout) : requestError);
}
