#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Network/Timer.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Queries/Requests/GetRosterRequest.h"

using namespace Swift;

SimpleEventLoop eventLoop;

Client* client = 0;
bool rosterReceived = false;

void printIncomingData(const String& data) {
	std::cout << "<- " << data << std::endl;
}

void printOutgoingData(const String& data) {
	std::cout << "-> " << data << std::endl;
}

void handleRosterReceived(boost::shared_ptr<Payload>) {
	rosterReceived = true;
	eventLoop.stop();
}

void handleConnected() {
	GetRosterRequest* rosterRequest = new GetRosterRequest(client, Request::AutoDeleteAfterResponse);
	rosterRequest->onResponse.connect(boost::bind(&handleRosterReceived, _1));
	rosterRequest->send();
}

int main(int, char**) {
	char* jid = getenv("SWIFT_CLIENTTEST_JID");
	if (!jid) {
		std::cerr << "Please set the SWIFT_CLIENTTEST_JID environment variable" << std::endl;
		return -1;
	}
	char* pass = getenv("SWIFT_CLIENTTEST_PASS");
	if (!pass) {
		std::cerr << "Please set the SWIFT_CLIENTTEST_PASS environment variable" << std::endl;
		return -1;
	}

	client = new Swift::Client(JID(jid), String(pass));
	client->onConnected.connect(&handleConnected);
	client->onDataRead.connect(&printIncomingData);
	client->onDataWritten.connect(&printOutgoingData);
	client->connect();

	{
		Timer timer(10000);
		timer.onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer.start();

		eventLoop.run();
	}
	delete client;
	return !rosterReceived;
}
