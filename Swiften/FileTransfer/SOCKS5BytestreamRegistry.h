/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <vector>
#include <set>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamClientSession.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
	class SOCKS5BytestreamRegistry {
		public:
			SOCKS5BytestreamRegistry();

			boost::shared_ptr<ReadBytestream> getReadBytestream(const std::string& destination) const;
			void addReadBytestream(const std::string& destination, boost::shared_ptr<ReadBytestream> byteStream);
			void removeReadBytestream(const std::string& destination);

			boost::shared_ptr<WriteBytestream> getWriteBytestream(const std::string& destination) const;
			void addWriteBytestream(const std::string& destination, boost::shared_ptr<WriteBytestream> byteStream);
			void removeWriteBytestream(const std::string& destination);

			/**
			 * Generate a new session ID to use for new S5B streams.
			 */
			std::string generateSessionID();

			/**
			 *	Start an actual transfer.
			 */
			SOCKS5BytestreamServerSession* getConnectedSession(const std::string& destination);

		public:
			static std::string getHostname(const std::string& sessionID, const JID& requester, const JID& target);

		private:
			friend class SOCKS5BytestreamServerSession;

			typedef std::map<std::string, boost::shared_ptr<ReadBytestream> > ReadBytestreamMap;
			ReadBytestreamMap readBytestreams;

			typedef std::map<std::string, boost::shared_ptr<WriteBytestream> > WriteBytestreamMap;
			WriteBytestreamMap writeBytestreams;

			std::map<std::string, SOCKS5BytestreamServerSession*> serverSessions;

			IDGenerator idGenerator;
	};
}
