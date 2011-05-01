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

class EchoBot {
	public:
		EchoBot(NetworkFactories* networkFactories) {
			client = new Client("echobot@wonderland.lit", "mypass", networkFactories);
			client->setAlwaysTrustCertificates();
			client->onConnected.connect(bind(&EchoBot::handleConnected, this));
			client->onMessageReceived.connect(
					bind(&EchoBot::handleMessageReceived, this, _1));
			tracer = new ClientXMLTracer(client);
			client->connect();
		}

		~EchoBot() {
			delete tracer;
			delete client;
		}
	
	private:
		void handleConnected() {
			std::cout << "Connected" << std::endl;
		}

		void handleMessageReceived(Message::ref message) {
			// Echo back the incoming message
			message->setTo(message->getFrom());
			message->setFrom(JID());
			client->sendMessage(message);
		}
	
	private:
		Client* client;
		ClientXMLTracer* tracer;
};

int main(int, char**) {
	SimpleEventLoop eventLoop;
	BoostNetworkFactories networkFactories(&eventLoop);

	EchoBot bot(&networkFactories);

	eventLoop.run();
	return 0;
}
