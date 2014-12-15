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
#include <Swiften/FileTransfer/SOCKS5BytestreamServerInitializeRequest.h>

static const unsigned int LOCAL_PREFERENCE = 0;

namespace Swift {

LocalJingleTransportCandidateGenerator::LocalJingleTransportCandidateGenerator(
		SOCKS5BytestreamServerManager* s5bServerManager,
		SOCKS5BytestreamProxiesManager* s5bProxy, 
		const JID& ownJID,
		IDGenerator* idGenerator) : 
			s5bServerManager(s5bServerManager),
			s5bProxy(s5bProxy), 
			ownJID(ownJID),
			idGenerator(idGenerator) {
}

LocalJingleTransportCandidateGenerator::~LocalJingleTransportCandidateGenerator() {
	SWIFT_LOG_ASSERT(!s5bServerInitializeRequest, warning) << std::endl;
}

void LocalJingleTransportCandidateGenerator::start() {
	assert(!s5bServerInitializeRequest);
	s5bServerInitializeRequest = s5bServerManager->createInitializeRequest();
	s5bServerInitializeRequest->onFinished.connect(
			boost::bind(&LocalJingleTransportCandidateGenerator::handleS5BServerInitialized, this, _1));

	s5bServerInitializeRequest->start();
}

void LocalJingleTransportCandidateGenerator::stop() {
	if (s5bServerInitializeRequest) {
		s5bServerInitializeRequest->stop();
		s5bServerInitializeRequest.reset();
	}
}

void LocalJingleTransportCandidateGenerator::handleS5BServerInitialized(bool success) {
	std::vector<JingleS5BTransportPayload::Candidate> candidates;
	if (success) {
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
	else {
		SWIFT_LOG(warning) << "Unable to start SOCKS5 server" << std::endl;
	}

	s5bServerInitializeRequest->stop();
	s5bServerInitializeRequest.reset();

	onLocalTransportCandidatesGenerated(candidates);
}

/*void LocalJingleTransportCandidateGenerator::handleS5BProxiesDiscovered() {
	foreach(S5BProxyRequest::ref proxy, s5bProxiesDiscoverRequest->getS5BProxies()) {
		if (proxy->getStreamHost()) { // FIXME: Added this test, because there were cases where this wasn't initialized. Investigate this. (Remko)
			JingleS5BTransportPayload::Candidate candidate;
			candidate.type = JingleS5BTransportPayload::Candidate::ProxyType;
			candidate.jid = (*proxy->getStreamHost()).jid;
			candidate.hostPort = (*proxy->getStreamHost()).addressPort;
			candidate.priority = 65536 * 10 + LOCAL_PREFERENCE;
			candidate.cid = idGenerator.generateID();
			s5bCandidatesPayload->addCandidate(candidate);
		}
	}

	haveS5BProxyCandidates = true;
	s5bProxiesDiscoverRequest->stop();
	s5bProxiesDiscoverRequest.reset();

	checkS5BCandidatesReady();
}*/

}
