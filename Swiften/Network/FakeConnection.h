/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include "Swiften/Network/Connection.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/EventLoop/EventLoop.h"

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

			FakeConnection(EventLoop* eventLoop) : eventLoop(eventLoop), state(Initial), delayConnect(false) {}

			virtual void listen() {
				assert(false);
			}

			virtual HostAddressPort getLocalAddress() const {
				return HostAddressPort();
			}

			void setError(const Error& e) {
				error = boost::optional<Error>(e);
				state = DisconnectedWithError;
				if (connectedTo) {
					eventLoop->postEvent(
							boost::bind(boost::ref(onDisconnected), error),
							shared_from_this());
				}
			}

			virtual void connect(const HostAddressPort& address) {
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
							boost::bind(boost::ref(onConnectFinished), error),
							shared_from_this());
				}
			}

			virtual void disconnect() {
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

			virtual void write(const ByteArray& data) {
				dataWritten.push_back(data);
			}

			void setDelayConnect() {
				delayConnect = true;
			}

			EventLoop* eventLoop;
			boost::optional<HostAddressPort> connectedTo;
			std::vector<ByteArray> dataWritten;
			boost::optional<Error> error;
			State state;
			bool delayConnect;
	};
}
