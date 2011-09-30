/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/FileTransfer/FileTransferManagerImpl.h>

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

#include <Swiften/Base/foreach.h>
#include "Swiften/Disco/EntityCapsProvider.h"
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxyFinder.h>
#include <Swiften/FileTransfer/ConnectivityManager.h>
#include <Swiften/FileTransfer/OutgoingFileTransferManager.h>
#include <Swiften/FileTransfer/IncomingFileTransferManager.h>
#include <Swiften/FileTransfer/DefaultLocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/DefaultRemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/ConnectionServerFactory.h>
#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NATTraverser.h>

namespace Swift {

FileTransferManagerImpl::FileTransferManagerImpl(const JID& ownFullJID, JingleSessionManager* jingleSessionManager, IQRouter* router, EntityCapsProvider* capsProvider, PresenceOracle* presOracle, ConnectionFactory* connectionFactory, ConnectionServerFactory* connectionServerFactory, TimerFactory* timerFactory, NATTraverser* natTraverser) : ownJID(ownFullJID), jingleSM(jingleSessionManager), iqRouter(router), capsProvider(capsProvider), presenceOracle(presOracle), timerFactory(timerFactory), connectionFactory(connectionFactory), connectionServerFactory(connectionServerFactory), natTraverser(natTraverser), bytestreamServer(NULL), s5bProxyFinder(NULL) {
	assert(!ownFullJID.isBare());

	connectivityManager = new ConnectivityManager(natTraverser);
	bytestreamRegistry = new SOCKS5BytestreamRegistry();
	bytestreamProxy = new SOCKS5BytestreamProxy(connectionFactory, timerFactory);

	localCandidateGeneratorFactory = new DefaultLocalJingleTransportCandidateGeneratorFactory(connectivityManager, bytestreamRegistry, bytestreamProxy, ownFullJID);
	remoteCandidateSelectorFactory = new DefaultRemoteJingleTransportCandidateSelectorFactory(connectionFactory, timerFactory);
	outgoingFTManager = new OutgoingFileTransferManager(jingleSM, iqRouter, capsProvider, remoteCandidateSelectorFactory, localCandidateGeneratorFactory, bytestreamRegistry, bytestreamProxy);
	incomingFTManager = new IncomingFileTransferManager(jingleSM, iqRouter, remoteCandidateSelectorFactory, localCandidateGeneratorFactory, bytestreamRegistry, bytestreamProxy, timerFactory);
	incomingFTManager->onIncomingFileTransfer.connect(onIncomingFileTransfer);
}

FileTransferManagerImpl::~FileTransferManagerImpl() {
	if (s5bProxyFinder) {
		s5bProxyFinder->stop();
		delete s5bProxyFinder;
	}
	if (bytestreamServer) {
		bytestreamServer->stop();
		delete bytestreamServer;
	}
	delete incomingFTManager;
	delete outgoingFTManager;
	delete remoteCandidateSelectorFactory;
	delete localCandidateGeneratorFactory;
	delete connectivityManager;
}

void FileTransferManagerImpl::startListeningOnPort(int port) {
	// TODO: create a server for each interface we're on
	SWIFT_LOG(debug) << "Start listening on port " << port << " and hope it's not in use." << std::endl;
	boost::shared_ptr<ConnectionServer> server = connectionServerFactory->createConnectionServer(HostAddress("0.0.0.0"), port);
	server->start();
	bytestreamServer = new SOCKS5BytestreamServer(server, bytestreamRegistry);
	bytestreamServer->start();
	connectivityManager->addListeningPort(port);

	s5bProxyFinder = new SOCKS5BytestreamProxyFinder(ownJID.getDomain(), iqRouter);
	s5bProxyFinder->onProxyFound.connect(boost::bind(&FileTransferManagerImpl::addS5BProxy, this, _1));
	s5bProxyFinder->start();
}

void FileTransferManagerImpl::addS5BProxy(S5BProxyRequest::ref proxy) {
	bytestreamProxy->addS5BProxy(proxy);
}

boost::optional<JID> FileTransferManagerImpl::highestPriorityJIDSupportingFileTransfer(const JID& bareJID) {
	JID fullReceipientJID;
	int priority = INT_MIN;
	
	//getAllPresence(bareJID) gives you all presences for the bare JID (i.e. all resources) Remko Tronçon @ 11:11
	std::vector<Presence::ref> presences = presenceOracle->getAllPresence(bareJID);

	//iterate over them
	foreach(Presence::ref pres, presences) {
		if (pres->getPriority() > priority) {
			// look up caps from the jid
			DiscoInfo::ref info = capsProvider->getCaps(pres->getFrom());
			if (info && info->hasFeature(DiscoInfo::JingleFeature) && info->hasFeature(DiscoInfo::JingleFTFeature) && (info->hasFeature(DiscoInfo::JingleTransportsIBBFeature) || info->hasFeature(DiscoInfo::JingleTransportsS5BFeature))) {
			
				priority = pres->getPriority();
				fullReceipientJID = pres->getFrom();
			}
		}
	}
	
	return fullReceipientJID.isValid() ? boost::optional<JID>(fullReceipientJID) : boost::optional<JID>();
}

OutgoingFileTransfer::ref FileTransferManagerImpl::createOutgoingFileTransfer(const JID& to, const boost::filesystem::path& filepath, const std::string& description, boost::shared_ptr<ReadBytestream> bytestream) {
	std::string filename = filepath.filename();
	boost::uintmax_t sizeInBytes = boost::filesystem::file_size(filepath);
	boost::posix_time::ptime lastModified = boost::posix_time::from_time_t(boost::filesystem::last_write_time(filepath));
	return createOutgoingFileTransfer(to, filename, description, sizeInBytes, lastModified, bytestream);
}

OutgoingFileTransfer::ref FileTransferManagerImpl::createOutgoingFileTransfer(const JID& to, const std::string& filename, const std::string& description, const boost::uintmax_t sizeInBytes, const boost::posix_time::ptime& lastModified, boost::shared_ptr<ReadBytestream> bytestream) {
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
	
	return outgoingFTManager->createOutgoingFileTransfer(ownJID, receipient, bytestream, fileInfo);
}

}
