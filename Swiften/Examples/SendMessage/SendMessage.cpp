#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Network/BoostTimer.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Client/ClientXMLTracer.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/MainBoostIOServiceThread.h"

using namespace Swift;

SimpleEventLoop eventLoop;

Client* client = 0;
JID recipient;
std::string messageBody;

void handleConnected() {
	boost::shared_ptr<Message> message(new Message());
	message->setBody(messageBody);
	message->setTo(recipient);
	client->sendMessage(message);
	client->disconnect();
	eventLoop.stop();
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <jid> <password> <recipient> <message>" << std::endl;
		return -1;
	}

	recipient = JID(argv[3]);
	messageBody = std::string(argv[4]);

	client = new Swift::Client(JID(argv[1]), String(argv[2]));
	ClientXMLTracer* tracer = new ClientXMLTracer(client);
	client->onConnected.connect(&handleConnected);
	client->connect();

	{
		boost::shared_ptr<BoostTimer> timer(new BoostTimer(30000, &MainBoostIOServiceThread::getInstance().getIOService()));
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();

		eventLoop.run();
	}

	delete tracer;
	delete client;
}
