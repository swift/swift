/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Network/BoostNetworkFactories.h"
#include "Swiften/EventLoop/EventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Roster/GetRosterRequest.h"
#include "Swiften/Client/ClientXMLTracer.h"

using namespace Swift;

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);

Client* client = 0;
bool reconnected = false;
bool rosterReceived = false;

void handleRosterReceived(boost::shared_ptr<Payload>) {
	if (reconnected) {
		rosterReceived = true;
		client->disconnect();
		eventLoop.stop();
	}
	else {
		reconnected = true;
		client->disconnect();
		client->connect();
	}
}

void handleConnected() {
	GetRosterRequest::ref rosterRequest = GetRosterRequest::create(client->getIQRouter());
	rosterRequest->onResponse.connect(boost::bind(&handleRosterReceived, _1));
	rosterRequest->send();
}

int main(int, char**) {
	char* jid = getenv("SWIFT_CLIENTTEST_JID");
	if (!jid) {
		std::cerr << "Please set the SWIFT_CLIENTTEST_JID environment variable" << std::endl;
		return -1;
	}
	char* pass = getenv("SWIFT_CLIENTTEST_PASS");
	if (!pass) {
		std::cerr << "Please set the SWIFT_CLIENTTEST_PASS environment variable" << std::endl;
		return -1;
	}

	client = new Swift::Client(&eventLoop, &networkFactories, JID(jid), String(pass));
	ClientXMLTracer* tracer = new ClientXMLTracer(client);
	client->onConnected.connect(&handleConnected);
	client->setAlwaysTrustCertificates();
	client->connect();

	{
		Timer::ref timer = networkFactories.getTimerFactory()->createTimer(30000);
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();

		eventLoop.run();
	}

	delete tracer;
	delete client;
	return !rosterReceived;
}
