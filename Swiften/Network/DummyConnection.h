/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cassert>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class DummyConnection : public Connection, public EventOwner,	public boost::enable_shared_from_this<DummyConnection> {
		public:
			void listen() {
				assert(false);
			}

			void connect(const HostAddressPort&) {
				assert(false);
			}

			void disconnect() {
				//assert(false);
			}

			void write(const ByteArray& data) {
				MainEventLoop::postEvent(boost::ref(onDataWritten), shared_from_this());
				onDataSent(data);
			}

			void receive(const ByteArray& data) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onDataRead), ByteArray(data)), shared_from_this());
			}

			HostAddressPort getLocalAddress() const {
				return localAddress;
			}

			boost::signal<void (const ByteArray&)> onDataSent;

			HostAddressPort localAddress;
	};
}
