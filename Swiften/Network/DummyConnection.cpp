/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/DummyConnection.h>

#include <cassert>
#include <memory>

#include <boost/bind.hpp>

namespace Swift {

DummyConnection::DummyConnection(EventLoop* eventLoop) : eventLoop(eventLoop) {
}

void DummyConnection::receive(const SafeByteArray& data) {
    eventLoop->postEvent(boost::bind(boost::ref(onDataRead), std::make_shared<SafeByteArray>(data)), shared_from_this());
}

void DummyConnection::listen() {
    assert(false);
}

void DummyConnection::connect(const HostAddressPort&) {
    assert(false);
}


}
