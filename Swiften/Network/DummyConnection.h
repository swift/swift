/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class DummyConnection : public Connection, public EventOwner,	public boost::enable_shared_from_this<DummyConnection> {
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

			boost::signal<void (const SafeByteArray&)> onDataSent;

			EventLoop* eventLoop;
			HostAddressPort localAddress;
	};
}
