/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <thread>

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Roster/GetRosterRequest.h>

using namespace Swift;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static Client* client = nullptr;
static bool rosterReceived = false;
enum TestStage {
    FirstConnect,
    Reconnect
};
static TestStage stage;
static ClientOptions options;

static void handleDisconnected(boost::optional<ClientError> e) {
    std::cout << "Disconnected: " << (e ? e.get().getType() : ClientError::UnknownError) << std::endl;
    if (stage == FirstConnect) {
        stage = Reconnect;
        client->connect(options);
    }
    else {
        eventLoop.stop();
    }
}

static void handleRosterReceived(std::shared_ptr<Payload>) {
    rosterReceived = true;
    std::cout << "Disconnecting" << std::endl;
    client->disconnect();
}

static void handleConnected() {
    std::cout << "Connected" << std::endl;
    rosterReceived = false;
    GetRosterRequest::ref rosterRequest = GetRosterRequest::create(client->getIQRouter());
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

    char* boshHost = getenv("SWIFT_CLIENTTEST_BOSH_HOST");
    char* boshPort = getenv("SWIFT_CLIENTTEST_BOSH_PORT");
    char* boshPath = getenv("SWIFT_CLIENTTEST_BOSH_PATH");

    if (boshHost && boshPort && boshPath) {
        std::cout << "Using BOSH with URL: http://" << boshHost << ":" << boshPort << boshPath << std::endl;
        try {
            options.boshURL = URL("http", boshHost, boost::numeric_cast<unsigned short>(boost::lexical_cast<int>(boshPort)), boshPath);
        }
        catch (const boost::numeric::bad_numeric_cast& e) {
            std::cerr << "SWIFT_CLIENTTEST_BOSH_PORT doesn't hold a valid port number: " << e.what() << std::endl;
            return -1;
        }
        catch (const boost::bad_lexical_cast& e) {
            std::cerr << "SWIFT_CLIENTTEST_BOSH_PORT doesn't hold a valid port number: " << e.what() << std::endl;
            return -1;
        }
    }

    client = new Swift::Client(JID(jid), std::string(pass), &networkFactories);
    ClientXMLTracer* tracer = new ClientXMLTracer(client, !options.boshURL.isEmpty());
    client->onConnected.connect(&handleConnected);
    client->onDisconnected.connect(boost::bind(&handleDisconnected, _1));
    client->setAlwaysTrustCertificates();
    stage = FirstConnect;
    client->connect(options);

    {
        Timer::ref timer = networkFactories.getTimerFactory()->createTimer(60000);
        timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
        timer->start();

        eventLoop.run();
    }

    delete tracer;
    delete client;
    return !rosterReceived;
}
