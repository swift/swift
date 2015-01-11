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

#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>

using namespace Swift;

RemoteJingleTransportCandidateSelector::RemoteJingleTransportCandidateSelector(
		ConnectionFactory* connectionFactory, 
		TimerFactory* timerFactory,
		const FileTransferOptions& options) :
			connectionFactory(connectionFactory), 
			timerFactory(timerFactory),
			options(options) {
}

RemoteJingleTransportCandidateSelector::~RemoteJingleTransportCandidateSelector() {
}

void RemoteJingleTransportCandidateSelector::addCandidates(
		const std::vector<JingleS5BTransportPayload::Candidate>& candidates) {
	foreach(JingleS5BTransportPayload::Candidate c,  candidates) {
		this->candidates.push(c);
	}
}

void RemoteJingleTransportCandidateSelector::startSelectingCandidate() {
	tryNextCandidate();
}

void RemoteJingleTransportCandidateSelector::stopSelectingCandidate() {
	if (s5bSession) {
		sessionReadyConnection.disconnect();
		s5bSession->stop();
	}
}

void RemoteJingleTransportCandidateSelector::tryNextCandidate() {
	if (candidates.empty()) {
		SWIFT_LOG(debug) << "No more candidates" << std::endl;
		onCandidateSelectFinished(
				boost::optional<JingleS5BTransportPayload::Candidate>(), boost::shared_ptr<SOCKS5BytestreamClientSession>());
	} 
	else {
		lastCandidate = candidates.top();
		candidates.pop();
		SWIFT_LOG(debug) << "Trying candidate " << lastCandidate.cid << std::endl;
		if ((lastCandidate.type == JingleS5BTransportPayload::Candidate::DirectType && options.isDirectAllowed()) ||
			(lastCandidate.type == JingleS5BTransportPayload::Candidate::AssistedType && options.isAssistedAllowed()) ||
			(lastCandidate.type == JingleS5BTransportPayload::Candidate::ProxyType && options.isProxiedAllowed())) {
			boost::shared_ptr<Connection> connection = connectionFactory->createConnection();
			s5bSession = boost::make_shared<SOCKS5BytestreamClientSession>(
					connection, lastCandidate.hostPort, socks5DstAddr, timerFactory);
			sessionReadyConnection = s5bSession->onSessionReady.connect(
					boost::bind(&RemoteJingleTransportCandidateSelector::handleSessionReady, this, _1));
			s5bSession->start();
		} 
		else {
			SWIFT_LOG(debug) << "Can't handle this type of candidate" << std::endl;
			tryNextCandidate();
		}
	}
}

void RemoteJingleTransportCandidateSelector::handleSessionReady(bool error) {
	sessionReadyConnection.disconnect();
	if (error) {
		s5bSession.reset();
		tryNextCandidate();
	}
	else {
		onCandidateSelectFinished(lastCandidate, s5bSession);
	}
}

void RemoteJingleTransportCandidateSelector::setSOCKS5DstAddr(const std::string& socks5DstAddr) {
	this->socks5DstAddr = socks5DstAddr;
}
