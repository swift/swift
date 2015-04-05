/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ConnectionServer.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class SWIFTEN_API DummyConnectionServer : public ConnectionServer, public EventOwner, public boost::enable_shared_from_this<DummyConnectionServer> {
		public:
			DummyConnectionServer(EventLoop* /*eventLoop*/, int port) : localAddressPort(HostAddress(), port) {}
			DummyConnectionServer(EventLoop* /*eventLoop*/, const Swift::HostAddress& hostAddress, int port) : localAddressPort(hostAddress, port) {}
			virtual ~DummyConnectionServer() {}

			virtual HostAddressPort getAddressPort() const {
				return localAddressPort;
			}

			virtual boost::optional<Error> tryStart() {
				return boost::optional<Error>();
			}

			virtual void start() {

			}

			virtual void stop() {

			}

		private:
			HostAddressPort localAddressPort;
	};
}
