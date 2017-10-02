/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class SWIFTEN_API DummyConnection : public Connection, public EventOwner,    public std::enable_shared_from_this<DummyConnection> {
        public:
            DummyConnection(EventLoop* eventLoop);

            void listen();
            void connect(const HostAddressPort&);

            void disconnect() {
                //assert(false);
            }

            void write(const SafeByteArray& data) {
                eventLoop->postEvent(boost::ref(onDataWritten), shared_from_this());
                onDataSent(data);
            }

            void receive(const SafeByteArray& data);

            HostAddressPort getLocalAddress() const {
                return localAddress;
            }

            HostAddressPort getRemoteAddress() const {
                return remoteAddress;
            }

            boost::signals2::signal<void (const SafeByteArray&)> onDataSent;

            EventLoop* eventLoop;
            HostAddressPort localAddress;
            HostAddressPort remoteAddress;
    };
}
