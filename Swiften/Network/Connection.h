/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class HostAddressPort;

	class Connection {
		public:
			typedef boost::shared_ptr<Connection> ref;

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

		public:
			boost::signal<void (bool /* error */)> onConnectFinished;
			boost::signal<void (const boost::optional<Error>&)> onDisconnected;
			boost::signal<void (boost::shared_ptr<SafeByteArray>)> onDataRead;
			boost::signal<void ()> onDataWritten;
	};
}
