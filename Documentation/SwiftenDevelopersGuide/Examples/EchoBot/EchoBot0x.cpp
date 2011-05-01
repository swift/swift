/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Swiften.h>

using namespace Swift;

int main(int, char**) {
	// Set up the event loop and network classes
	SimpleEventLoop eventLoop;
	BoostNetworkFactories networkFactories(&eventLoop);

	Client client("echobot@wonderland.lit", "mypass", &networkFactories);
	client.setAlwaysTrustCertificates();
	client.onConnected.connect([&] {
		std::cout << "Connected" << std::endl;
	});
	client.onMessageReceived.connect([&] (Message::ref message) {
		message->setTo(message->getFrom());
		message->setFrom(JID());
		client.sendMessage(message);
	});
	client.connect();

	eventLoop.run();

	return 0;
}
