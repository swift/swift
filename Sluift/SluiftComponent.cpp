/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/SluiftComponent.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Component/ComponentXMLTracer.h>
#include <Swiften/Component/Component.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Sluift/SluiftGlobals.h>
#include <Sluift/Lua/Exception.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Queries/RawRequest.h>
#include <Sluift/Helpers.h>
#include <Swiften/Elements/Presence.h>

using namespace Swift;

SluiftComponent::SluiftComponent(
		const JID& jid, 
		const std::string& password, 
		NetworkFactories* networkFactories, 
		SimpleEventLoop* eventLoop):
			networkFactories(networkFactories), 
			eventLoop(eventLoop),
			tracer(NULL) {
	component = new Component(jid, password, networkFactories);
	component->onError.connect(boost::bind(&SluiftComponent::handleError, this, _1));
	component->onMessageReceived.connect(boost::bind(&SluiftComponent::handleIncomingMessage, this, _1));
	component->onPresenceReceived.connect(boost::bind(&SluiftComponent::handleIncomingPresence, this, _1));
}

SluiftComponent::~SluiftComponent() {
	delete tracer;
	delete component;
}

void SluiftComponent::connect(const std::string& host, int port) {
	disconnectedError = boost::optional<ComponentError>();
	component->connect(host, port);
}

void SluiftComponent::setTraceEnabled(bool b) {
	if (b && !tracer) {
		tracer = new ComponentXMLTracer(component);
	}
	else if (!b && tracer) {
		delete tracer;
		tracer = NULL;
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
	while (component->isAvailable()) {
		eventLoop->runUntilEvents();
	}
}

void SluiftComponent::setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os) {
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
		while (!watchdog.getTimedOut() && currentIndex >= pendingEvents.size() && component->isAvailable()) {
			eventLoop->runUntilEvents();
		}

		// Finish if we're disconnected or timed out
		if (watchdog.getTimedOut() || !component->isAvailable()) {
			return boost::optional<Event>();
		}
	}
}

void SluiftComponent::handleIncomingMessage(boost::shared_ptr<Message> stanza) {
	pendingEvents.push_back(Event(stanza));
}

void SluiftComponent::handleIncomingPresence(boost::shared_ptr<Presence> stanza) {
	pendingEvents.push_back(Event(stanza));
}

void SluiftComponent::handleRequestResponse(boost::shared_ptr<Payload> response, boost::shared_ptr<ErrorPayload> error) {
	requestResponse = response;
	requestError = error;
	requestResponseReceived = true;
}

void SluiftComponent::handleError(const boost::optional<ComponentError>& error) {
	disconnectedError = error;
}

Sluift::Response SluiftComponent::doSendRequest(boost::shared_ptr<Request> request, int timeout) {
	requestResponse.reset();
	requestError.reset();
	requestResponseReceived = false;
	request->send();

	Watchdog watchdog(timeout, networkFactories->getTimerFactory());
	while (!watchdog.getTimedOut() && !requestResponseReceived) {
		eventLoop->runUntilEvents();
	}
	return Sluift::Response(requestResponse, watchdog.getTimedOut() ? 
			boost::make_shared<ErrorPayload>(ErrorPayload::RemoteServerTimeout) : requestError);
}
