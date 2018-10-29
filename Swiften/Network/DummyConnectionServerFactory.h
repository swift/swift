/*
 * Copyright (c) 2014-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Network/ConnectionServerFactory.h>
#include <Swiften/Network/DummyConnectionServer.h>

namespace Swift {

class EventLoop;

class DummyConnectionServerFactory : public ConnectionServerFactory {
public:
    DummyConnectionServerFactory(EventLoop* eventLoop) : eventLoop(eventLoop) {}
    virtual ~DummyConnectionServerFactory() {}

    virtual std::shared_ptr<ConnectionServer> createConnectionServer(unsigned short port) {
        return std::make_shared<DummyConnectionServer>(eventLoop, port);
    }

    virtual std::shared_ptr<ConnectionServer> createConnectionServer(const Swift::HostAddress& hostAddress, unsigned short port) {
        return std::make_shared<DummyConnectionServer>(eventLoop, hostAddress, port);
    }

private:
    EventLoop* eventLoop;
};

}
