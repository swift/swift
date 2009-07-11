#include <boost/bind.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"

using namespace Swift;
using namespace boost;

class EchoBot {
	public:
		EchoBot(const JID& jid, const String& pass) : client(new Client(jid, pass)) {
			client->onConnected.connect(bind(&EchoBot::handleConnected, this));
			client->onMessageReceived.connect(bind(&EchoBot::handleMessageReceived, this, _1));
			client->connect();
		}

	private:
		void handleConnected() {
			shared_ptr<GetRosterRequest> rosterRequest(new GetRosterRequest(client.get()));
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
		std::auto_ptr<Client> client;
};

int main(int, char**) {
	SimpleEventLoop eventLoop;
	EchoBot bot(JID("echobot@wonderland.lit"), "mypass");
	eventLoop.run();
	return 0;
}
