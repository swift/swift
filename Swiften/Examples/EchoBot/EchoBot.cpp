/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Client/Client.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"

using namespace Swift;
using namespace boost;

class EchoBot {
	public:
		EchoBot(const JID& jid, const String& pass) {
			client = new Client(jid, pass);
			client->onConnected.connect(bind(&EchoBot::handleConnected, this));
			client->onMessageReceived.connect(bind(&EchoBot::handleMessageReceived, this, _1));
			client->connect();
		}

		~EchoBot() {
			delete client;
		}

	private:
		void handleConnected() {
			shared_ptr<GetRosterRequest> rosterRequest(new GetRosterRequest(client));
			rosterRequest->onResponse.connect(bind(&EchoBot::handleRosterReceived, this, _2));
			rosterRequest->send();
		}

		void handleRosterReceived(const optional<ErrorPayload>& error) {
			if (error) {
				std::cerr << "Error receiving roster. Continuing anyway.";
			}
			client->sendPresence(shared_ptr<Presence>(new Presence("Send me a message")));
		}

		void handleMessageReceived(shared_ptr<Message> message) {
			message->setTo(message->getFrom());
			message->setFrom(JID());
			client->sendMessage(message);
		}

	private:
		Client* client;
};

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <jid> <pass>" << std::endl;
		return -1;
	}
	SimpleEventLoop eventLoop;
	EchoBot bot(JID(argv[1]), argv[2]);
	eventLoop.run();
	return 0;
}
