/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <boost/bind.hpp>

#include <Swiften/Swiften.h>

using namespace Swift;
using namespace boost;

Client* client;

void handleConnected();
void handleMessageReceived(Message::ref message);

int main(int, char**) {
	SimpleEventLoop eventLoop;
	BoostNetworkFactories networkFactories(&eventLoop);

	client = new Client("echobot@wonderland.lit", "mypass", &networkFactories);
	client->setAlwaysTrustCertificates();
	client->onConnected.connect(&handleConnected);
	client->onMessageReceived.connect(bind(&handleMessageReceived, _1));
	client->connect();

	eventLoop.run();

	delete client;
	return 0;
}

void handleConnected() {
	std::cout << "Connected" << std::endl;
}

void handleMessageReceived(Message::ref message) {
	// Echo back the incoming message
	message->setTo(message->getFrom());
	message->setFrom(JID());
	client->sendMessage(message);
}
