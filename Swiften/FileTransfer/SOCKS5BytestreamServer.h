/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/FileTransfer/ReadBytestream.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamRegistry.h"

namespace Swift {
	class SOCKS5BytestreamServerSession;

	class SOCKS5BytestreamServer {
		public:
			SOCKS5BytestreamServer(boost::shared_ptr<ConnectionServer> connectionServer);

			HostAddressPort getAddressPort() const;

			void start();
			void stop();

			void addBytestream(const String& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> byteStream);
			void removeBytestream(const String& id, const JID& from, const JID& to);

		/*protected:
			boost::shared_ptr<ReadBytestream> getBytestream(const String& dest);*/

		private:
			void handleNewConnection(boost::shared_ptr<Connection> connection);

			static String getSOCKSDestinationAddress(const String& id, const JID& from, const JID& to);

		private:
			friend class SOCKS5BytestreamServerSession;

			boost::shared_ptr<ConnectionServer> connectionServer;
			SOCKS5BytestreamRegistry bytestreams;
			std::vector<boost::shared_ptr<SOCKS5BytestreamServerSession> > sessions;
	};
}

