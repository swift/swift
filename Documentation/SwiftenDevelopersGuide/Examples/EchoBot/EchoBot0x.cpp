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

	// Initialize the client with the JID and password
	Client client("echobot@wonderland.lit", "mypass", &networkFactories);

	// When the client is convnected, send out initial presence
	client.onConnected.connect([&] {
		client.sendPresence(Presence::create("Send me a message"));
	});

	// When the client receives an incoming message, echo it back
	client.onMessageReceived.connect([&] (Message::ref message) {
		message->setTo(message->getFrom());
		message->setFrom(JID());
		client.sendMessage(message);
	});

	// Start the client
	client.connect();

	// Run the event loop to start processing incoming network events
	eventLoop.run();
	return 0;
}
