/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/EventLoop/EventLoop.h>

namespace Swift {
	class FakeConnection : 
			public Connection, 
			public EventOwner,
			public boost::enable_shared_from_this<FakeConnection> {
		public:
			enum State {
				Initial,
				Connecting,
				Connected,
				Disconnected,
				DisconnectedWithError
			};

			FakeConnection(EventLoop* eventLoop);
			~FakeConnection();

			virtual void listen();
			virtual HostAddressPort getLocalAddress() const {
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
