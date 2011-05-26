/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

#include <Swiften/Network/ConnectionServer.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>

namespace Swift {
	class SOCKS5BytestreamServerSession;

	class SOCKS5BytestreamServer {
		public:
			SOCKS5BytestreamServer(boost::shared_ptr<ConnectionServer> connectionServer, SOCKS5BytestreamRegistry* registry);

			HostAddressPort getAddressPort() const;

			void start();
			void stop();

			void addReadBytestream(const std::string& id, const JID& from, const JID& to, boost::shared_ptr<ReadBytestream> byteStream);
			void removeReadBytestream(const std::string& id, const JID& from, const JID& to);

		/*protected:
			boost::shared_ptr<ReadBytestream> getBytestream(const std::string& dest);*/

		private:
			void handleNewConnection(boost::shared_ptr<Connection> connection);

			static std::string getSOCKSDestinationAddress(const std::string& id, const JID& from, const JID& to);

		private:
			friend class SOCKS5BytestreamServerSession;

			boost::shared_ptr<ConnectionServer> connectionServer;
			SOCKS5BytestreamRegistry* registry;
			std::vector<boost::shared_ptr<SOCKS5BytestreamServerSession> > sessions;
	};
}

