/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "DefaultRemoteJingleTransportCandidateSelector.h"

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>

namespace Swift {

DefaultRemoteJingleTransportCandidateSelector::DefaultRemoteJingleTransportCandidateSelector(ConnectionFactory* connectionFactory, TimerFactory* timerFactory) : connectionFactory(connectionFactory), timerFactory(timerFactory) {
}

DefaultRemoteJingleTransportCandidateSelector::~DefaultRemoteJingleTransportCandidateSelector() {
}

void DefaultRemoteJingleTransportCandidateSelector::addRemoteTransportCandidates(JingleTransportPayload::ref transportPayload) {
	JingleS5BTransportPayload::ref s5bPayload;
	transportSID = transportPayload->getSessionID();
	if ((s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transportPayload))) {
		foreach(JingleS5BTransportPayload::Candidate c,  s5bPayload->getCandidates()) {
			candidates.push(c);
		}
	}
}

void DefaultRemoteJingleTransportCandidateSelector::selectCandidate() {
	tryNextCandidate(true);
}

void DefaultRemoteJingleTransportCandidateSelector::tryNextCandidate(bool error) {
	if (error) {
		if (s5bSession) {
			SWIFT_LOG(debug) << "failed to connect" << std::endl;
		}
		if (candidates.empty()) {
			// failed to connect to any of the candidates
			// issue an error
			SWIFT_LOG(debug) << "out of candidates )=" << std::endl;
			JingleS5BTransportPayload::ref failed = boost::make_shared<JingleS5BTransportPayload>();
			failed->setCandidateError(true);
			failed->setSessionID(transportSID);
			onRemoteTransportCandidateSelectFinished(failed);
		} else {
			lastCandidate = candidates.top();
			// only try direct or assisted for now
			if (lastCandidate.type == JingleS5BTransportPayload::Candidate::DirectType ||
				lastCandidate.type == JingleS5BTransportPayload::Candidate::AssistedType || lastCandidate.type == JingleS5BTransportPayload::Candidate::ProxyType ) {
				// create connection
				connection = connectionFactory->createConnection();
				s5bSession = boost::make_shared<SOCKS5BytestreamClientSession>(connection, lastCandidate.hostPort, SOCKS5BytestreamRegistry::getHostname(transportSID, requester, target), timerFactory);

				// bind onReady to this method
				s5bSession->onSessionReady.connect(boost::bind(&DefaultRemoteJingleTransportCandidateSelector::tryNextCandidate, this, _1));

				std::string candidateType;
				if (lastCandidate.type == JingleS5BTransportPayload::Candidate::DirectType) {
					candidateType = "direct";
				} else if (lastCandidate.type == JingleS5BTransportPayload::Candidate::AssistedType) {
					candidateType = "assisted";
				} else if (lastCandidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
					candidateType = "proxy";
				}

				// initiate connect
				SWIFT_LOG(debug) << "try to connect to candidate of type " << candidateType << " : " << lastCandidate.hostPort.toString() << std::endl;
				s5bSession->start();

				// that's it. we're gonna be called back
				candidates.pop();
			} else {
				s5bSession.reset();
				candidates.pop();
				tryNextCandidate(true);
			}
		}
	} else {
		// we have a working connection, hooray
		JingleS5BTransportPayload::ref success = boost::make_shared<JingleS5BTransportPayload>();
		success->setCandidateUsed(lastCandidate.cid);
		success->setSessionID(transportSID);
		onRemoteTransportCandidateSelectFinished(success);
	}
}

void DefaultRemoteJingleTransportCandidateSelector::setMinimumPriority(int priority) {
	SWIFT_LOG(debug) << "priority: " << priority << std::endl;
}

void DefaultRemoteJingleTransportCandidateSelector::setRequesterTargtet(const JID& requester, const JID& target) {
	this->requester = requester;
	this->target = target;
}

SOCKS5BytestreamClientSession::ref DefaultRemoteJingleTransportCandidateSelector::getS5BSession() {
	return s5bSession;
}

bool DefaultRemoteJingleTransportCandidateSelector::isActualCandidate(JingleTransportPayload::ref /* transportPayload */) {
	return false;
}

int DefaultRemoteJingleTransportCandidateSelector::getPriority(JingleTransportPayload::ref /* transportPayload */) {
	return 0;
}

JingleTransport::ref DefaultRemoteJingleTransportCandidateSelector::selectTransport(JingleTransportPayload::ref /* transportPayload */) {
	return JingleTransport::ref();
}

}
