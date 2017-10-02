/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/FakeConnection.h>

#include <boost/bind.hpp>

namespace Swift {

FakeConnection::FakeConnection(EventLoop* eventLoop) : eventLoop(eventLoop), state(Initial), delayConnect(false) {
}

FakeConnection::~FakeConnection() {
}

void FakeConnection::listen() {
    assert(false);
}

void FakeConnection::setError(const Error& e) {
    error = boost::optional<Error>(e);
    state = DisconnectedWithError;
    if (connectedTo) {
        eventLoop->postEvent(
                boost::bind(boost::ref(onDisconnected), error),
                shared_from_this());
    }
}

void FakeConnection::connect(const HostAddressPort& address) {
    if (delayConnect) {
        state = Connecting;
    }
    else {
        if (!error) {
            connectedTo = address;
            state = Connected;
        }
        else {
            state = DisconnectedWithError;
        }
        eventLoop->postEvent(
                boost::bind(boost::ref(onConnectFinished), error ? true : false),
                shared_from_this());
    }
}

void FakeConnection::disconnect() {
    if (!error) {
        state = Disconnected;
    }
    else {
        state = DisconnectedWithError;
    }
    connectedTo.reset();
    eventLoop->postEvent(
            boost::bind(boost::ref(onDisconnected), error),
            shared_from_this());
}

}
