/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/JID/JID.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/Elements/S5BProxyRequest.h>

namespace Swift {
	class ConnectionFactory;
	class ConnectionServerFactory;
	class SOCKS5BytestreamServerManager;
	class EntityCapsProvider;
	class IQRouter;
	class IncomingFileTransferManager;
	class JingleSessionManager;
	class OutgoingFileTransferManager;
	class NATTraverser;
	class PresenceOracle;
	class ReadBytestream;
	class FileTransferTransporterFactory;
	class SOCKS5BytestreamRegistry;
	class SOCKS5BytestreamProxiesManager;
	class TimerFactory;
	class CryptoProvider;
	class NetworkEnvironment;

	class SWIFTEN_API FileTransferManagerImpl : public FileTransferManager {
		public:
			FileTransferManagerImpl(
					const JID& ownFullJID, 
					JingleSessionManager* jingleSessionManager, 
					IQRouter* router, 
					EntityCapsProvider* capsProvider, 
					PresenceOracle* presOracle, 
					ConnectionFactory* connectionFactory,
					ConnectionServerFactory* connectionServerFactory,
					TimerFactory* timerFactory, 
					NetworkEnvironment* networkEnvironment,
					NATTraverser* natTraverser,
					CryptoProvider* crypto);
			~FileTransferManagerImpl();
			
			OutgoingFileTransfer::ref createOutgoingFileTransfer(
					const JID& to, 
					const boost::filesystem::path& filepath, 
					const std::string& description, 
					boost::shared_ptr<ReadBytestream> bytestream,
					const FileTransferOptions&) SWIFTEN_OVERRIDE;
			OutgoingFileTransfer::ref createOutgoingFileTransfer(
					const JID& to, 
					const std::string& filename, 
					const std::string& description, 
					const boost::uintmax_t sizeInBytes, 
					const boost::posix_time::ptime& lastModified, 
					boost::shared_ptr<ReadBytestream> bytestream,
					const FileTransferOptions&) SWIFTEN_OVERRIDE;

			void start();
			void stop();
			
		private:
			boost::optional<JID> highestPriorityJIDSupportingFileTransfer(const JID& bareJID);
			
		private:
			JID ownJID;
			
			OutgoingFileTransferManager* outgoingFTManager;
			IncomingFileTransferManager* incomingFTManager;
			FileTransferTransporterFactory* transporterFactory;
			IQRouter* iqRouter;
			EntityCapsProvider* capsProvider;
			PresenceOracle* presenceOracle;
			IDGenerator idGenerator;
			SOCKS5BytestreamRegistry* bytestreamRegistry;
			SOCKS5BytestreamProxiesManager* bytestreamProxy;
			SOCKS5BytestreamServerManager* s5bServerManager;
	};
}
