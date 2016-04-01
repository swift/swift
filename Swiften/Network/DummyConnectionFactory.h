/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyConnection.h>

namespace Swift {

class EventLoop;

class DummyConnectionFactory : public ConnectionFactory {
public:
    DummyConnectionFactory(EventLoop *eventLoop) : eventLoop(eventLoop) {}
    virtual ~DummyConnectionFactory() {}
    virtual std::shared_ptr<Connection> createConnection() {
        return std::make_shared<DummyConnection>(eventLoop);
    }
private:
    EventLoop* eventLoop;
};

}
