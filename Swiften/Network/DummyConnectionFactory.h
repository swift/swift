/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyConnection.h>

namespace Swift {

class EventLoop;

class DummyConnectionFactory : public ConnectionFactory {
public:
	DummyConnectionFactory(EventLoop *eventLoop) : eventLoop(eventLoop) {}
	virtual ~DummyConnectionFactory() {}
	virtual boost::shared_ptr<Connection> createConnection() {
		return boost::make_shared<DummyConnection>(eventLoop);
	}
private:
	EventLoop* eventLoop;
};

}
