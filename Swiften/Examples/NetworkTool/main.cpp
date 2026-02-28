/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>

#include <boost/lexical_cast.hpp>

#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>

using namespace Swift;

static SimpleEventLoop eventLoop;

static void handleGetPublicIPRequestResponse(const boost::optional<HostAddress>& result) {
    if (result) {
        std::cerr << "Result: " << result->toString() << std::endl;;
    }
    else {
        std::cerr << "No result" << std::endl;
    }
    eventLoop.stop();
}

static void handleGetForwardPortRequestResponse(const boost::optional<NATPortMapping>& result) {
    if (result) {
        std::cerr << "Result: " << result->getPublicPort() << " -> " << result->getLocalPort() << std::endl;;
    }
    else {
        std::cerr << "No result" << std::endl;
    }
    eventLoop.stop();
}

static void handleRemovePortForwardingRequestResponse(const boost::optional<bool> result) {
    if (result && result.get()) {
        std::cerr << "Result: OK" << std::endl;
    }
    else {
        std::cerr << "Result: ERROR" << std::endl;
    }
    eventLoop.stop();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Invalid parameters" << std::endl;
        return -1;
    }

    PlatformNATTraversalWorker natTraverser(&eventLoop);
    if (std::string(argv[1]) == "get-public-ip") {
        std::shared_ptr<NATTraversalGetPublicIPRequest> query = natTraverser.createGetPublicIPRequest();
        query->onResult.connect(boost::bind(&handleGetPublicIPRequestResponse, _1));
        query->start();
        eventLoop.run();
    }
    else if (std::string(argv[1]) == "add-port-forward") {
        if (argc < 4) {
            std::cerr << "Invalid parameters" << std::endl;
        }
        std::shared_ptr<NATTraversalForwardPortRequest> query = natTraverser.createForwardPortRequest(boost::lexical_cast<int>(argv[2]), boost::lexical_cast<int>(argv[3]));
        query->onResult.connect(boost::bind(&handleGetForwardPortRequestResponse, _1));
        query->start();
        eventLoop.run();
    }
    else if (std::string(argv[1]) == "remove-port-forward") {
        if (argc < 4) {
            std::cerr << "Invalid parameters" << std::endl;
        }
        std::shared_ptr<NATTraversalRemovePortForwardingRequest> query = natTraverser.createRemovePortForwardingRequest(boost::lexical_cast<int>(argv[2]), boost::lexical_cast<int>(argv[3]));
        query->onResult.connect(boost::bind(&handleRemovePortForwardingRequestResponse, _1));
        query->start();
        eventLoop.run();
    }
    else if (std::string(argv[1]) == "get-local-ip") {
        std::cout << PlatformNetworkEnvironment().getLocalAddress().toString() << std::endl;
    }
}
