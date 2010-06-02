/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_signalslib.h"

#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class HostAddressPort;

	class Connection {
		public:
			enum Error {
				ReadError,
				WriteError
			};

			Connection() {}
			virtual ~Connection() {}

			virtual void listen() = 0;
			virtual void connect(const HostAddressPort& address) = 0;
			virtual void disconnect() = 0;
			virtual void write(const ByteArray& data) = 0;

		public:
			boost::signal<void (bool /* error */)> onConnectFinished;
			boost::signal<void (const boost::optional<Error>&)> onDisconnected;
			boost::signal<void (const ByteArray&)> onDataRead;
	};
}
