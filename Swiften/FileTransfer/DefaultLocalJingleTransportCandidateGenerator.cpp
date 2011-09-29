/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "DefaultLocalJingleTransportCandidateGenerator.h"

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/ConnectivityManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>

namespace Swift {

DefaultLocalJingleTransportCandidateGenerator::DefaultLocalJingleTransportCandidateGenerator(ConnectivityManager* connectivityManager, SOCKS5BytestreamRegistry* s5bRegistry, SOCKS5BytestreamProxy* s5bProxy, JID& ownJID) : connectivityManager(connectivityManager), s5bRegistry(s5bRegistry), s5bProxy(s5bProxy), ownJID(ownJID) {
}

DefaultLocalJingleTransportCandidateGenerator::~DefaultLocalJingleTransportCandidateGenerator() {
}

void DefaultLocalJingleTransportCandidateGenerator::generateLocalTransportCandidates(JingleTransportPayload::ref transportPayload) {
	if (boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transportPayload)) {
		JingleTransportPayload::ref payL = boost::make_shared<JingleTransportPayload>();
		payL->setSessionID(transportPayload->getSessionID());
		onLocalTransportCandidatesGenerated(payL);
	}
	if (boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transportPayload)) {
		JingleS5BTransportPayload::ref payL = boost::make_shared<JingleS5BTransportPayload>();
		payL->setSessionID(transportPayload->getSessionID());
		payL->setMode(JingleS5BTransportPayload::TCPMode);

		const unsigned long localPreference = 0;

		// get direct candidates
		std::vector<HostAddressPort> directCandidates = connectivityManager->getHostAddressPorts();
		foreach(HostAddressPort addressPort, directCandidates) {
			JingleS5BTransportPayload::Candidate candidate;
			candidate.type = JingleS5BTransportPayload::Candidate::DirectType;
			candidate.jid = ownJID;
			candidate.hostPort = addressPort;
			candidate.priority = 65536 * 126 + localPreference;
			candidate.cid = idGenerator.generateID();
			payL->addCandidate(candidate);
		}

		// get assissted candidates
		std::vector<HostAddressPort> assisstedCandidates = connectivityManager->getAssistedHostAddressPorts();
		foreach(HostAddressPort addressPort, assisstedCandidates) {
			JingleS5BTransportPayload::Candidate candidate;
			candidate.type = JingleS5BTransportPayload::Candidate::AssistedType;
			candidate.jid = ownJID;
			candidate.hostPort = addressPort;
			candidate.priority = 65536 * 120 + localPreference;
			candidate.cid = idGenerator.generateID();
			payL->addCandidate(candidate);
		}

		// get proxy candidates
		std::vector<S5BProxyRequest::ref> proxyCandidates = s5bProxy->getS5BProxies();
		foreach(S5BProxyRequest::ref proxy, proxyCandidates) {
			if (proxy->getStreamHost()) { // FIXME: Added this test, because there were cases where this wasn't initialized. Investigate this. (Remko)
				JingleS5BTransportPayload::Candidate candidate;
				candidate.type = JingleS5BTransportPayload::Candidate::ProxyType;
				candidate.jid = (*proxy->getStreamHost()).jid;
				candidate.hostPort = (*proxy->getStreamHost()).addressPort;
				candidate.priority = 65536 * 10 + localPreference;
				candidate.cid = idGenerator.generateID();
				payL->addCandidate(candidate);
			}
		}

		onLocalTransportCandidatesGenerated(payL);
	}
	
}

bool DefaultLocalJingleTransportCandidateGenerator::isActualCandidate(JingleTransportPayload::ref transportPayload) {
	if (!transportPayload.get()) return false;
	return false;
}

int DefaultLocalJingleTransportCandidateGenerator::getPriority(JingleTransportPayload::ref /* transportPayload */) {
	return 0;
}

JingleTransport::ref DefaultLocalJingleTransportCandidateGenerator::selectTransport(JingleTransportPayload::ref /* transportPayload */) {
	return JingleTransport::ref();
}

}
