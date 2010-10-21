/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/Client/Client.h"
#include "Swiften/Network/BoostTimer.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Client/ClientXMLTracer.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/MainBoostIOServiceThread.h"
#include "Swiften/Disco/GetDiscoInfoRequest.h"

using namespace Swift;

enum ExitCodes {OK = 0, CANNOT_CONNECT, CANNOT_AUTH, NO_RESPONSE, DISCO_ERROR};

SimpleEventLoop eventLoop;

Client* client = 0;
JID recipient;
int exitCode = CANNOT_CONNECT;
boost::bsignals::connection errorConnection;

void handleServerDiscoInfoResponse(boost::shared_ptr<DiscoInfo> /*info*/, const boost::optional<ErrorPayload>& error) {
	if (!error) {
		errorConnection.disconnect();
		client->disconnect();
		eventLoop.stop();
		exitCode = OK;
	} else {
		errorConnection.disconnect();
		exitCode = DISCO_ERROR;
	}
}

void handleConnected() {
	exitCode = NO_RESPONSE;
	GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(JID(), client->getIQRouter());
	discoInfoRequest->onResponse.connect(handleServerDiscoInfoResponse);
	discoInfoRequest->send();
}

void handleError(const ClientError&) {
	exitCode = CANNOT_AUTH;
	eventLoop.stop();
}



int main(int argc, char* argv[]) {
	if (argc < 4 || argc > 5) {
		std::cerr << "Usage: " << argv[0] << " <jid> [<connect_host>] <password> <timeout_seconds>" << std::endl;
		return -1;
	}

	int argi = 1;
	
	String jid = argv[argi++];
	String connectHost = "";
	if (argc == 5) {
		connectHost = argv[argi++];
	}

	client = new Swift::Client(JID(jid), String(argv[argi++]));
	char* timeoutChar = argv[argi++];
	int timeout = atoi(timeoutChar);
	timeout = (timeout ? timeout : 30) * 1000;
	ClientXMLTracer* tracer = new ClientXMLTracer(client);
	client->onConnected.connect(&handleConnected);
	errorConnection = client->onError.connect(&handleError);
	std::cout << "Connecting to JID " << jid << " with timeout " << timeout << "ms on host: "; ;
	if (!connectHost.isEmpty()) {
		std::cout << connectHost << std::endl;
		client->connect(connectHost);
	} else {
		std::cout << " Default" << std::endl;
		client->connect();
	}

	{
		BoostTimer::ref timer(BoostTimer::create(timeout, &MainBoostIOServiceThread::getInstance().getIOService()));
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();

		eventLoop.run();
	}

	delete tracer;
	delete client;
	return exitCode;
}
