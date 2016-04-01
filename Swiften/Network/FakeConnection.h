/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class SWIFTEN_API FakeConnection :
            public Connection,
            public EventOwner,
            public std::enable_shared_from_this<FakeConnection> {
        public:
            enum State {
                Initial,
                Connecting,
                Connected,
                Disconnected,
                DisconnectedWithError
            };

            FakeConnection(EventLoop* eventLoop);
            virtual ~FakeConnection();

            virtual void listen();
            virtual HostAddressPort getLocalAddress() const {
                return HostAddressPort();
            }
            virtual HostAddressPort getRemoteAddress() const {
                return HostAddressPort();
            }

            void setError(const Error& e);
            virtual void connect(const HostAddressPort& address);
            virtual void disconnect();

            virtual void write(const SafeByteArray& data) {
                dataWritten.push_back(data);
            }

            void setDelayConnect() {
                delayConnect = true;
            }

            EventLoop* eventLoop;
            boost::optional<HostAddressPort> connectedTo;
            std::vector<SafeByteArray> dataWritten;
            boost::optional<Error> error;
            State state;
            bool delayConnect;
    };
}
