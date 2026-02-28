/*
 * Copyright (c) 2014-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/SluiftComponent.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Component/Component.h>
#include <Swiften/Component/ComponentXMLTracer.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Queries/RawRequest.h>
#include <Swiften/Roster/XMPPRoster.h>

#include <Sluift/Helpers.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/SluiftGlobals.h>

using namespace Swift;

SluiftComponent::SluiftComponent(
        const JID& jid,
        const std::string& password,
        NetworkFactories* networkFactories,
        SimpleEventLoop* eventLoop):
            networkFactories(networkFactories),
            eventLoop(eventLoop),
            tracer(nullptr) {
    component = new Component(jid, password, networkFactories);
    component->onError.connect(boost::bind(&SluiftComponent::handleError, this, _1));
    component->onMessageReceived.connect(boost::bind(&SluiftComponent::handleIncomingMessage, this, _1));
    component->onPresenceReceived.connect(boost::bind(&SluiftComponent::handleIncomingPresence, this, _1));
}

SluiftComponent::~SluiftComponent() {
    delete tracer;
    delete component;
}

void SluiftComponent::connect(const std::string& host, unsigned short port) {
    if (component->isActive()) {
        throw Lua::Exception("Component is already connecting or connected");
    }
    disconnectedError = boost::optional<ComponentError>();
    component->connect(host, port);
}

void SluiftComponent::setTraceEnabled(bool b) {
    if (b && !tracer) {
        tracer = new ComponentXMLTracer(component);
    }
    else if (!b && tracer) {
        delete tracer;
        tracer = nullptr;
    }
}

void SluiftComponent::waitConnected(int timeout) {
    Watchdog watchdog(timeout, networkFactories->getTimerFactory());
    while (!watchdog.getTimedOut() && !disconnectedError && !component->isAvailable()) {
        eventLoop->runUntilEvents();
    }
    if (watchdog.getTimedOut()) {
        component->disconnect();
        throw Lua::Exception("Timeout while connecting");
    }
    if (disconnectedError) {
        throw Lua::Exception(getErrorString(*disconnectedError));
    }
}

bool SluiftComponent::isConnected() const {
    return component->isAvailable();
}

void SluiftComponent::disconnect() {
    component->disconnect();
    while (component->isActive()) {
        eventLoop->runUntilEvents();
    }
}

void SluiftComponent::setSoftwareVersion(const std::string& name, const std::string& version, const std::string& /* os */) {
    component->setSoftwareVersion(name, version);
}

boost::optional<SluiftComponent::Event> SluiftComponent::getNextEvent(
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
        while (!watchdog.getTimedOut() && currentIndex >= pendingEvents.size() && component->isActive()) {
            eventLoop->runUntilEvents();
        }

        // Finish if we're disconnected or timed out
        if (watchdog.getTimedOut() || !component->isActive()) {
            return boost::optional<Event>();
        }
    }
}

void SluiftComponent::handleIncomingMessage(std::shared_ptr<Message> stanza) {
    pendingEvents.push_back(Event(stanza));
}

void SluiftComponent::handleIncomingPresence(std::shared_ptr<Presence> stanza) {
    pendingEvents.push_back(Event(stanza));
}

void SluiftComponent::handleRequestResponse(std::shared_ptr<Payload> response, std::shared_ptr<ErrorPayload> error) {
    requestResponse = response;
    requestError = error;
    requestResponseReceived = true;
}

void SluiftComponent::handleError(const boost::optional<ComponentError>& error) {
    disconnectedError = error;
}

Sluift::Response SluiftComponent::doSendRequest(std::shared_ptr<Request> request, int timeout) {
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
