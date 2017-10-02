/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <thread>

#include <boost/bind.hpp>

#include <Swiften/Base/sleep.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Roster/GetRosterRequest.h>
#include <Swiften/TLS/BlindCertificateTrustChecker.h>

using namespace Swift;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);
static int numberOfConnectedClients = 0;
static int numberOfInstances = 100;


static void handleConnected() {
    numberOfConnectedClients++;
    std::cout << "Connected " << numberOfConnectedClients << std::endl;
}

int main(int, char**) {
    char* jid = getenv("SWIFT_BENCHTOOL_JID");
    if (!jid) {
        std::cerr << "Please set the SWIFT_BENCHTOOL_JID environment variable" << std::endl;
        return -1;
    }
    char* pass = getenv("SWIFT_BENCHTOOL_PASS");
    if (!pass) {
        std::cerr << "Please set the SWIFT_BENCHTOOL_PASS environment variable" << std::endl;
        return -1;
    }

    BlindCertificateTrustChecker trustChecker;
    std::vector<CoreClient*> clients;
    for (int i = 0; i < numberOfInstances; ++i) {
        CoreClient* client = new Swift::CoreClient(JID(jid), createSafeByteArray(std::string(pass)), &networkFactories);
        client->setCertificateTrustChecker(&trustChecker);
        client->onConnected.connect(&handleConnected);
        clients.push_back(client);
    }

    for (auto& client : clients) {
        client->connect();
    }

    eventLoop.run();

    for (auto& client : clients) {
        delete client;
    }

    return 0;
}
