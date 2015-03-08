/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerResourceUser.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerPortForwardingUser.h>

static const unsigned int LOCAL_PREFERENCE = 0;

namespace Swift {

LocalJingleTransportCandidateGenerator::LocalJingleTransportCandidateGenerator(
		SOCKS5BytestreamServerManager* s5bServerManager,
		SOCKS5BytestreamProxiesManager* s5bProxy, 
		const JID& ownJID,
		IDGenerator* idGenerator,
		const FileTransferOptions& options) :
			s5bServerManager(s5bServerManager),
			s5bProxy(s5bProxy), 
			ownJID(ownJID),
			idGenerator(idGenerator),
			options_(options),
			triedServerInit_(false),
			triedForwarding_(false),
			triedProxyDiscovery_(false) {
}

LocalJingleTransportCandidateGenerator::~LocalJingleTransportCandidateGenerator() {
	SWIFT_LOG_ASSERT(!s5bServerInitializeRequest, warning) << std::endl;
}

void LocalJingleTransportCandidateGenerator::start() {
	assert(!s5bServerInitializeRequest);
	if (options_.isDirectAllowed() || options_.isAssistedAllowed()) {
		s5bServerResourceUser_ = s5bServerManager->aquireResourceUser();
		if (s5bServerResourceUser_->isInitialized()) {
			handleS5BServerInitialized(true);
		}
		else {
			s5bServerResourceUser_->onSuccessfulInitialized.connect(boost::bind(&LocalJingleTransportCandidateGenerator::handleS5BServerInitialized, this, _1));
		}
	} else {
		handleS5BServerInitialized(false);
	}

	if (options_.isProxiedAllowed()) {
		s5bProxy->onDiscoveredProxiesChanged.connect(boost::bind(&LocalJingleTransportCandidateGenerator::handleDiscoveredProxiesChanged, this));
		if (s5bProxy->getOrDiscoverS5BProxies().is_initialized()) {
			handleDiscoveredProxiesChanged();
		}
	}
}

void LocalJingleTransportCandidateGenerator::stop() {
	s5bServerResourceUser_.reset();
}

void LocalJingleTransportCandidateGenerator::handleS5BServerInitialized(bool success) {
	triedServerInit_ = true;
	if (success) {
		if (options_.isAssistedAllowed()) {
			// try to setup port forwarding
			s5bServerPortForwardingUser_ = s5bServerManager->aquirePortForwardingUser();
			if (s5bServerPortForwardingUser_->isForwardingSetup()) {
				handlePortForwardingSetup(true);
			}
		}
	}
	else {
		SWIFT_LOG(warning) << "Unable to start SOCKS5 server" << std::endl;
		s5bServerResourceUser_.reset();
		handlePortForwardingSetup(false);
	}
	checkS5BCandidatesReady();
}

void LocalJingleTransportCandidateGenerator::handlePortForwardingSetup(bool success) {
	triedForwarding_ = true;
	checkS5BCandidatesReady();
}

void LocalJingleTransportCandidateGenerator::handleDiscoveredProxiesChanged() {
	s5bProxy->onDiscoveredProxiesChanged.disconnect(boost::bind(&LocalJingleTransportCandidateGenerator::handleDiscoveredProxiesChanged, this));
	triedProxyDiscovery_ = true;
	checkS5BCandidatesReady();
}

void LocalJingleTransportCandidateGenerator::checkS5BCandidatesReady() {
	if ((!options_.isDirectAllowed()  || (options_.isDirectAllowed()  && triedServerInit_)) &&
		(!options_.isProxiedAllowed() || (options_.isProxiedAllowed() && triedProxyDiscovery_)) &&
		(!options_.isDirectAllowed()  || (options_.isDirectAllowed()  && triedServerInit_))) {
		emitOnLocalTransportCandidatesGenerated();
	}
}


void LocalJingleTransportCandidateGenerator::emitOnLocalTransportCandidatesGenerated() {
	std::vector<JingleS5BTransportPayload::Candidate> candidates;

	if (options_.isDirectAllowed()) {
		// get direct candidates
		std::vector<HostAddressPort> directCandidates = s5bServerManager->getHostAddressPorts();
		foreach(HostAddressPort addressPort, directCandidates) {
			JingleS5BTransportPayload::Candidate candidate;
			candidate.type = JingleS5BTransportPayload::Candidate::DirectType;
			candidate.jid = ownJID;
			candidate.hostPort = addressPort;
			candidate.priority = 65536 * 126 + LOCAL_PREFERENCE;
			candidate.cid = idGenerator->generateID();
			candidates.push_back(candidate);
		}
	}

	if (options_.isAssistedAllowed()) {
		// get assissted candidates
		std::vector<HostAddressPort> assisstedCandidates = s5bServerManager->getAssistedHostAddressPorts();
		foreach(HostAddressPort addressPort, assisstedCandidates) {
			JingleS5BTransportPayload::Candidate candidate;
			candidate.type = JingleS5BTransportPayload::Candidate::AssistedType;
			candidate.jid = ownJID;
			candidate.hostPort = addressPort;
			candidate.priority = 65536 * 120 + LOCAL_PREFERENCE;
			candidate.cid = idGenerator->generateID();
			candidates.push_back(candidate);
		}
	}

	if (options_.isProxiedAllowed() && s5bProxy->getOrDiscoverS5BProxies().is_initialized()) {
		foreach(S5BProxyRequest::ref proxy, s5bProxy->getOrDiscoverS5BProxies().get()) {
			if (proxy->getStreamHost()) { // FIXME: Added this test, because there were cases where this wasn't initialized. Investigate this. (Remko)
				JingleS5BTransportPayload::Candidate candidate;
				candidate.type = JingleS5BTransportPayload::Candidate::ProxyType;
				candidate.jid = (*proxy->getStreamHost()).jid;
				HostAddress address = (*proxy->getStreamHost()).host;
				assert(address.isValid());
				candidate.hostPort = HostAddressPort(address, (*proxy->getStreamHost()).port);
				candidate.priority = 65536 * 10 + LOCAL_PREFERENCE;
				candidate.cid = idGenerator->generateID();
				candidates.push_back(candidate);
			}
		}
	}

	onLocalTransportCandidatesGenerated(candidates);
}

}
