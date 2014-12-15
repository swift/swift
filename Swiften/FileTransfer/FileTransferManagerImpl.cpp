/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileTransferManagerImpl.h>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/cstdint.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/Path.h>
#include "Swiften/Disco/EntityCapsProvider.h"
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/OutgoingFileTransferManager.h>
#include <Swiften/FileTransfer/IncomingFileTransferManager.h>
#include <Swiften/FileTransfer/DefaultFileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/ConnectionServerFactory.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NATTraverser.h>

#include <Swiften/Base/BoostFilesystemVersion.h>

namespace Swift {

FileTransferManagerImpl::FileTransferManagerImpl(
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
		CryptoProvider* crypto) : 
			ownJID(ownFullJID), 
			iqRouter(router), 
			capsProvider(capsProvider), 
			presenceOracle(presOracle) {
	assert(!ownFullJID.isBare());

	bytestreamRegistry = new SOCKS5BytestreamRegistry();
	s5bServerManager = new SOCKS5BytestreamServerManager(
			bytestreamRegistry, connectionServerFactory, networkEnvironment, natTraverser);
	bytestreamProxy = new SOCKS5BytestreamProxiesManager(connectionFactory, timerFactory);

	transporterFactory = new DefaultFileTransferTransporterFactory(
			bytestreamRegistry,
			s5bServerManager,
			bytestreamProxy,
			&idGenerator,
			connectionFactory, 
			timerFactory, 
			crypto,
			iqRouter);
	outgoingFTManager = new OutgoingFileTransferManager(
			jingleSessionManager, 
			iqRouter, 
			transporterFactory,
			crypto);
	incomingFTManager = new IncomingFileTransferManager(
			jingleSessionManager, 
			iqRouter, 
			transporterFactory,
			timerFactory,
			crypto);
	incomingFTManager->onIncomingFileTransfer.connect(onIncomingFileTransfer);
}

FileTransferManagerImpl::~FileTransferManagerImpl() {
	delete s5bServerManager;
	delete incomingFTManager;
	delete outgoingFTManager;
	delete transporterFactory;
}

void FileTransferManagerImpl::start() {
}

void FileTransferManagerImpl::stop() {
	s5bServerManager->stop();
}

boost::optional<JID> FileTransferManagerImpl::highestPriorityJIDSupportingFileTransfer(const JID& bareJID) {
	JID fullReceipientJID;
	int priority = INT_MIN;
	
	//getAllPresence(bareJID) gives you all presences for the bare JID (i.e. all resources) Isode Limited. @ 11:11
	std::vector<Presence::ref> presences = presenceOracle->getAllPresence(bareJID);

	//iterate over them
	foreach(Presence::ref pres, presences) {
		if (pres->getPriority() > priority) {
			// look up caps from the jid
			DiscoInfo::ref info = capsProvider->getCaps(pres->getFrom());
			if (isSupportedBy(info)) {
				priority = pres->getPriority();
				fullReceipientJID = pres->getFrom();
			}
		}
	}
	
	return fullReceipientJID.isValid() ? boost::optional<JID>(fullReceipientJID) : boost::optional<JID>();
}

OutgoingFileTransfer::ref FileTransferManagerImpl::createOutgoingFileTransfer(
		const JID& to, 
		const boost::filesystem::path& filepath, 
		const std::string& description, 
		boost::shared_ptr<ReadBytestream> bytestream,
		const FileTransferOptions& config) {
#if BOOST_FILESYSTEM_VERSION == 2 // TODO: Delete this when boost 1.44 becomes a minimum requirement, and we no longer need v2
	std::string filename = filepath.filename();
#else
	std::string filename = pathToString(filepath.filename());
#endif

	boost::uintmax_t sizeInBytes = boost::filesystem::file_size(filepath);
	boost::posix_time::ptime lastModified = boost::posix_time::from_time_t(boost::filesystem::last_write_time(filepath));
	return createOutgoingFileTransfer(to, filename, description, sizeInBytes, lastModified, bytestream, config);
}

OutgoingFileTransfer::ref FileTransferManagerImpl::createOutgoingFileTransfer(
		const JID& to, 
		const std::string& filename, 
		const std::string& description, 
		const boost::uintmax_t sizeInBytes, 
		const boost::posix_time::ptime& lastModified, 
		boost::shared_ptr<ReadBytestream> bytestream,
		const FileTransferOptions& config) {
	StreamInitiationFileInfo fileInfo;
	fileInfo.setDate(lastModified);
	fileInfo.setSize(sizeInBytes);
	fileInfo.setName(filename);
	fileInfo.setDescription(description);
	
	JID receipient = to;
	
	if(receipient.isBare()) {
		boost::optional<JID> fullJID = highestPriorityJIDSupportingFileTransfer(receipient);
		if (fullJID.is_initialized()) {
			receipient = fullJID.get();
		} else {
			return OutgoingFileTransfer::ref();
		}
	}
	
	return outgoingFTManager->createOutgoingFileTransfer(ownJID, receipient, bytestream, fileInfo, config);
}

}
