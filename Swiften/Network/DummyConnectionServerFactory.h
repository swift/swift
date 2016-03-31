/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Network/ConnectionServerFactory.h>
#include <Swiften/Network/DummyConnectionServer.h>

namespace Swift {

class EventLoop;

class DummyConnectionServerFactory : public ConnectionServerFactory {
public:
    DummyConnectionServerFactory(EventLoop* eventLoop) : eventLoop(eventLoop) {}
    virtual ~DummyConnectionServerFactory() {}

    virtual boost::shared_ptr<ConnectionServer> createConnectionServer(int port) {
        return boost::make_shared<DummyConnectionServer>(eventLoop, port);
    }

    virtual boost::shared_ptr<ConnectionServer> createConnectionServer(const Swift::HostAddress& hostAddress, int port) {
        return boost::make_shared<DummyConnectionServer>(eventLoop, hostAddress, port);
    }

private:
    EventLoop* eventLoop;
};

}
