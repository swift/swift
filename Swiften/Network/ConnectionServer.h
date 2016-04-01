/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional/optional_fwd.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class SWIFTEN_API ConnectionServer {
        public:
            enum Error {
                Conflict,
                UnknownError
            };

            virtual ~ConnectionServer();

            virtual HostAddressPort getAddressPort() const = 0;

            virtual boost::optional<Error> tryStart() = 0; // FIXME: This should become the new start

            virtual void start() = 0;

            virtual void stop() = 0;

            boost::signal<void (std::shared_ptr<Connection>)> onNewConnection;
    };
}
