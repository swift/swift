/*
 * Copyright (c) 2013-2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/SluiftClient.h>

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Sluift/SluiftGlobals.h>
#include <Sluift/Lua/Exception.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/PubSubEvent.h>
#include <Swiften/Queries/RawRequest.h>
#include <Sluift/ClientHelpers.h>
#include <Swiften/Elements/Presence.h>

using namespace Swift;

SluiftClient::SluiftClient(
		const JID& jid, 
		const std::string& password, 
		NetworkFactories* networkFactories, 
		SimpleEventLoop* eventLoop) :
			networkFactories(networkFactories), 
			eventLoop(eventLoop),
			tracer(NULL) {
	client = new Client(jid, password, networkFactories);
	client->setAlwaysTrustCertificates();
	client->onDisconnected.connect(boost::bind(&SluiftClient::handleDisconnected, this, _1));
	client->onMessageReceived.connect(boost::bind(&SluiftClient::handleIncomingMessage, this, _1));
	client->onPresenceReceived.connect(boost::bind(&SluiftClient::handleIncomingPresence, this, _1));
	client->getPubSubManager()->onEvent.connect(boost::bind(&SluiftClient::handleIncomingPubSubEvent, this, _1, _2));
	client->getRoster()->onInitialRosterPopulated.connect(boost::bind(&SluiftClient::handleInitialRosterPopulated, this));
}

SluiftClient::~SluiftClient() {
	delete tracer;
	delete client;
}

void SluiftClient::connect() {
	rosterReceived = false;
	disconnectedError = boost::optional<ClientError>();
	client->connect(options);
}

void SluiftClient::connect(const std::string& host, int port) {
	rosterReceived = false;
	options.manualHostname = host;
	options.manualPort = port;
	disconnectedError = boost::optional<ClientError>();
	client->connect(options);
}

void SluiftClient::setTraceEnabled(bool b) {
	if (b && !tracer) {
		tracer = new ClientXMLTracer(client, options.boshURL.isEmpty()? false: true);
	}
	else if (!b && tracer) {
		delete tracer;
		tracer = NULL;
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
		throw Lua::Exception(getClientErrorString(*disconnectedError));
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

std::vector<XMPPRosterItem> SluiftClient::getRoster() {
	if (!rosterReceived) {
		// If we haven't requested it yet, request it for the first time
		client->requestRoster();
	}
	while (!rosterReceived) {
		eventLoop->runUntilEvents();
	}
	return client->getRoster()->getItems();
}

void SluiftClient::handleIncomingMessage(boost::shared_ptr<Message> stanza) {
	if (stanza->getPayload<PubSubEvent>()) {
		// Already handled by pubsub manager
		return;
	}
	pendingEvents.push_back(Event(stanza));
}

void SluiftClient::handleIncomingPresence(boost::shared_ptr<Presence> stanza) {
	pendingEvents.push_back(Event(stanza));
}

void SluiftClient::handleIncomingPubSubEvent(const JID& from, boost::shared_ptr<PubSubEventPayload> event) {
	pendingEvents.push_back(Event(from, event));
}

void SluiftClient::handleInitialRosterPopulated() {
	rosterReceived = true;
}

void SluiftClient::handleRequestResponse(boost::shared_ptr<Payload> response, boost::shared_ptr<ErrorPayload> error) {
	requestResponse = response;
	requestError = error;
	requestResponseReceived = true;
}

void SluiftClient::handleDisconnected(const boost::optional<ClientError>& error) {
	disconnectedError = error;
}

Sluift::Response SluiftClient::doSendRequest(boost::shared_ptr<Request> request, int timeout) {
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
