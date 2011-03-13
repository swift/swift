/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/DummyConnection.h>

#include <boost/bind.hpp>
#include <cassert>

namespace Swift {

DummyConnection::DummyConnection(EventLoop* eventLoop) : eventLoop(eventLoop) {
}

void DummyConnection::receive(const ByteArray& data) {
	eventLoop->postEvent(boost::bind(boost::ref(onDataRead), ByteArray(data)), shared_from_this());
}

void DummyConnection::listen() {
	assert(false);
}

void DummyConnection::connect(const HostAddressPort&) {
	assert(false);
}


}
