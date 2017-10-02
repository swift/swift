/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <thread>

#include <boost/bind.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/GetDiscoInfoRequest.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>

using namespace Swift;

enum ExitCodes {OK = 0, CANNOT_CONNECT, CANNOT_AUTH, NO_RESPONSE, DISCO_ERROR};

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static Client* client = nullptr;
static JID recipient;
static int exitCode = CANNOT_CONNECT;
static boost::signals2::connection errorConnection;

static void handleServerDiscoInfoResponse(std::shared_ptr<DiscoInfo> /*info*/, ErrorPayload::ref error) {
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

static void handleConnected() {
    exitCode = NO_RESPONSE;
    GetDiscoInfoRequest::ref discoInfoRequest = GetDiscoInfoRequest::create(JID(), client->getIQRouter());
    discoInfoRequest->onResponse.connect(&handleServerDiscoInfoResponse);
    discoInfoRequest->send();
}

static void handleDisconnected(const boost::optional<ClientError>&) {
    exitCode = CANNOT_AUTH;
    eventLoop.stop();
}



int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " <jid> [<connect_host>] <password> <timeout_seconds>" << std::endl;
        return -1;
    }

    int argi = 1;

    std::string jid = argv[argi++];
    std::string connectHost = "";
    if (argc == 5) {
        connectHost = argv[argi++];
    }

    client = new Swift::Client(JID(jid), std::string(argv[argi++]), &networkFactories);
    char* timeoutChar = argv[argi++];
    int timeout = atoi(timeoutChar);
    timeout = (timeout ? timeout : 30) * 1000;
    ClientXMLTracer* tracer = new ClientXMLTracer(client);
    client->onConnected.connect(&handleConnected);
    errorConnection = client->onDisconnected.connect(&handleDisconnected);
    std::cout << "Connecting to JID " << jid << " with timeout " << timeout << "ms on host: "; ;
    if (!connectHost.empty()) {
        std::cout << connectHost << std::endl;
        ClientOptions options;
        options.manualHostname = connectHost;
        client->connect(options);
    } else {
        std::cout << " Default" << std::endl;
        client->connect();
    }

    {
        Timer::ref timer = networkFactories.getTimerFactory()->createTimer(timeout);
        timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
        timer->start();

        eventLoop.run();
    }

    delete tracer;
    delete client;
    return exitCode;
}
