/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Client/ClientXMLTracer.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"

using namespace Swift;
using namespace boost;

class EchoBot {
	public:
		EchoBot(const JID& jid, const String& pass) : tracer(0) {
			client = new Client(jid, pass);
			tracer = new ClientXMLTracer(client);
			client->onConnected.connect(bind(&EchoBot::handleConnected, this));
			client->onMessageReceived.connect(bind(&EchoBot::handleMessageReceived, this, _1));
			client->connect();
		}

		~EchoBot() {
			delete tracer;
			delete client;
		}

	private:
		void handleConnected() {
			shared_ptr<GetRosterRequest> rosterRequest(new GetRosterRequest(client));
			rosterRequest->onResponse.connect(bind(&EchoBot::handleRosterReceived, this, _2));
			rosterRequest->send();
		}

		void handleRosterReceived(const optional<Error>& error) {
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
		ClientXMLTracer* tracer;
};

int main(int, char**) {
	SimpleEventLoop eventLoop;
	EchoBot bot(JID("echobot@wonderland.lit"), "mypass");
	eventLoop.run();
	return 0;
}
