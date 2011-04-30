/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include <Swiften/Client/Client.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>

using namespace Swift;

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);

Client* client = 0;
JID recipient;
std::string messageBody;
int exitCode = 2;
boost::bsignals::connection errorConnection;


void handleConnected() {
	boost::shared_ptr<Message> message(new Message());
	message->setBody(messageBody);
	message->setTo(recipient);
	client->sendMessage(message);
	exitCode = 0;
	errorConnection.disconnect();
	client->disconnect();
	eventLoop.stop();
}

void handleDisconnected(const boost::optional<ClientError>&) {
	std::cerr << "Error!" << std::endl;
	exitCode = 1;
	eventLoop.stop();
}

int main(int argc, char* argv[]) {
	if (argc < 5 || argc > 6) {
		std::cerr << "Usage: " << argv[0] << " <jid> [<connect_host>]<password> <recipient> <message>" << std::endl;
		return -1;
	}

	int argi = 1;
	
	std::string jid = argv[argi++];
	std::string connectHost = "";
	if (argc == 6) {
		connectHost = argv[argi++];
	}

	client = new Swift::Client(JID(jid), std::string(argv[argi++]), &networkFactories);
	client->setAlwaysTrustCertificates();

	recipient = JID(argv[argi++]);
	messageBody = std::string(argv[argi++]);

	ClientXMLTracer* tracer = new ClientXMLTracer(client);
	client->onConnected.connect(&handleConnected);
	errorConnection = client->onDisconnected.connect(&handleDisconnected);
	if (!connectHost.empty()) {
		client->connect(connectHost);
	} else {
		client->connect();
	}

	{
		Timer::ref timer = networkFactories.getTimerFactory()->createTimer(30000);
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();

		eventLoop.run();
	}

	delete tracer;
	delete client;
	return exitCode;
}
