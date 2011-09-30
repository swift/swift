/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/Elements/S5BProxyRequest.h>

namespace Swift {
	class Client;
	class ConnectionFactory;
	class ConnectionServerFactory;
	class ConnectivityManager;
	class EntityCapsProvider;
	class IQRouter;
	class IncomingFileTransferManager;
	class JingleSessionManager;
	class LocalJingleTransportCandidateGeneratorFactory;
	class OutgoingFileTransferManager;
	class NATTraverser;
	class PresenceOracle;
	class ReadBytestream;
	class RemoteJingleTransportCandidateSelectorFactory;
	class SOCKS5BytestreamRegistry;
	class SOCKS5BytestreamServer;
	class SOCKS5BytestreamProxy;
	class TimerFactory;
	class SOCKS5BytestreamProxyFinder;

	class FileTransferManagerImpl : public FileTransferManager {
		public:
			FileTransferManagerImpl(const JID& ownFullJID, JingleSessionManager* jingleSessionManager, IQRouter* router, EntityCapsProvider* capsProvider, PresenceOracle* presOracle, ConnectionFactory* connectionFactory, ConnectionServerFactory* connectionServerFactory, TimerFactory* timerFactory, NATTraverser* natTraverser);
			~FileTransferManagerImpl();
			
			void startListeningOnPort(int port);
			void addS5BProxy(S5BProxyRequest::ref proxy);

			OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID& to, const boost::filesystem::path& filepath, const std::string& description, boost::shared_ptr<ReadBytestream> bytestream);
			OutgoingFileTransfer::ref createOutgoingFileTransfer(const JID& to, const std::string& filename, const std::string& description, const boost::uintmax_t sizeInBytes, const boost::posix_time::ptime& lastModified, boost::shared_ptr<ReadBytestream> bytestream);
			
		private:
			boost::optional<JID> highestPriorityJIDSupportingFileTransfer(const JID& bareJID);
			
		private:
			JID ownJID;
			
			OutgoingFileTransferManager* outgoingFTManager;
			IncomingFileTransferManager* incomingFTManager;
			RemoteJingleTransportCandidateSelectorFactory* remoteCandidateSelectorFactory;
			LocalJingleTransportCandidateGeneratorFactory* localCandidateGeneratorFactory;
			JingleSessionManager* jingleSM;
			IQRouter* iqRouter;
			EntityCapsProvider* capsProvider;
			PresenceOracle* presenceOracle;

			TimerFactory* timerFactory;
			ConnectionFactory* connectionFactory;
			ConnectionServerFactory* connectionServerFactory;
			NATTraverser* natTraverser;
			SOCKS5BytestreamRegistry* bytestreamRegistry;
			SOCKS5BytestreamServer* bytestreamServer;
			SOCKS5BytestreamProxy* bytestreamProxy;
			ConnectivityManager* connectivityManager;
			SOCKS5BytestreamProxyFinder* s5bProxyFinder;
	};

}
