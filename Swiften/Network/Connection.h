/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    class HostAddressPort;

    class SWIFTEN_API Connection {
        public:
            typedef std::shared_ptr<Connection> ref;

            enum Error {
                ReadError,
                WriteError
            };

            Connection();
            virtual ~Connection();

            virtual void listen() = 0;
            virtual void connect(const HostAddressPort& address) = 0;
            virtual void disconnect() = 0;
            virtual void write(const SafeByteArray& data) = 0;

            virtual HostAddressPort getLocalAddress() const = 0;
            virtual HostAddressPort getRemoteAddress() const = 0;

        public:
            boost::signals2::signal<void (bool /* error */)> onConnectFinished;
            boost::signals2::signal<void (const boost::optional<Error>&)> onDisconnected;
            boost::signals2::signal<void (std::shared_ptr<SafeByteArray>)> onDataRead;
            boost::signals2::signal<void ()> onDataWritten;
    };
}
