/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Network/BoostTimer.h"
#include "Swiften/EventLoop/EventLoop.h"
#include "Swiften/Client/ClientXMLTracer.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/MainBoostIOServiceThread.h"

using namespace Swift;

SimpleEventLoop eventLoop;

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

void handleError(const ClientError&) {
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
	
	String jid = argv[argi++];
	String connectHost = "";
	if (argc == 6) {
		connectHost = argv[argi++];
	}

	client = new Swift::Client(&eventLoop, JID(jid), String(argv[argi++]));

	recipient = JID(argv[argi++]);
	messageBody = std::string(argv[argi++]);

	ClientXMLTracer* tracer = new ClientXMLTracer(client);
	client->onConnected.connect(&handleConnected);
	errorConnection = client->onError.connect(&handleError);
	if (!connectHost.isEmpty()) {
		client->connect(connectHost);
	} else {
		client->connect();
	}

	{
		BoostTimer::ref timer(BoostTimer::create(30000, &MainBoostIOServiceThread::getInstance().getIOService(), &eventLoop));
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();

		eventLoop.run();
	}

	delete tracer;
	delete client;
	return exitCode;
}
